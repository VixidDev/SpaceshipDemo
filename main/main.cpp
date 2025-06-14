#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <map>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec2.hpp"
#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
#endif

#if defined(__GNUC__)
#	pragma GCC diagnostic ignored "-Wconversion"
#	pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define FONTSTASH_IMPLEMENTATION
#include "../third_party/fontstash/include/fontstash.h"
#define GLFONTSTASH_IMPLEMENTATION
#include "glfontstatsh.hpp"

#include "defaults.hpp"
#include "loadobj.hpp"
#include "texture.hpp"
#include "vaos.hpp"
#include "particles.hpp"

namespace
{
	constexpr char const* kWindowTitle = "Spaceship Flight Demo";

	// Constants

	Vec3f lightPositions[] = {
		Vec3f{2.f, -2.f, 5.f},
		Vec3f{-3.f, 0.f, 4.f},
		Vec3f{1.f, 5.f, 3.f}
	};
	Vec3f lightColors[] = {
		Vec3f{0.33f, 0.33f, 0.33f},
		Vec3f{0.66f, 0.66f, 0.66f},
		Vec3f{1.f, 1.f, 1.f}
	};

	Vec3f spaceshipPos = Vec3f{ 5.f, 4.2f, -20.f };

	// PI constant
	constexpr float PI = 3.1415926f;

	enum CameraState
	{
		FREE_CAM,
		FIXED_DISTANCE,
		GROUND_CAM
	};

	struct State_
	{
		float windowWidth;
		float windowHeight;

		bool splitScreen;

		// Delta time. Time in between frames
		float deltaTime;

		// Keeps a map of keys pressed to ensure
		// we can process them frame-independently
		std::map<int, std::pair<bool, int>> keys;

		ShaderProgram* mainProgram;
		ShaderProgram* blinnPhongProgram;
		ShaderProgram* particlesProgram;
		ShaderProgram* rectProgram;
		ShaderProgram* textProgram;

		GLuint terrainTextureID;
		GLuint particleTextureID;

		// Particle generator initialising
		ParticleGenerator generator{ 500, Vec3f{ 5.f, -5.1f, -20.f } };

		// Whether or not the flying animation is active or not
		bool animationActive;
		// The position delta to move the rocket
		Vec3f rocketPosDelta;
		// How long the animation has been active for
		float animationActiveFor;

		// Fontstash context
		FONScontext* fs;
		int font;

		// Colors for the liftoff and reset buttons
		Vec4f buttonOneColor;
		Vec4f buttonTwoColor;

		struct Camera
		{
			// Whether the camera is active or not
			bool active;

			// Camera state
			CameraState cameraStateMain;
			// In the case we are in split screen, this is the state of the camera
			// for the split screen view
			CameraState cameraStateSecondary;

			// World position of the camera
			Vec3f position;
			// A normalized vector pointing in the direction we want the camera to point
			Vec3f frontDirection;
			// The up vector, decides the orientation of the camera, usually always keep this as (0, 1, 0)
			Vec3f upVector;

			// How sensitive the mouse is when looking around
			float mouseSensitivity;
			// How fast the camera moves
			float moveSpeed;

			// Last x position of cursor
			float lastX;
			// Last y position of cursor
			float lastY;
			// Yaw angle
			float yaw;
			// Pitch angle
			float pitch;
			
			// Flag determining whether it is our first mouse callback or not
			bool firstMouse;
		} camera;
	};
	
	void glfw_callback_error_( int, char const* );

	void glfw_callback_key_( GLFWwindow*, int, int, int, int );

	void glfw_callback_motion_(GLFWwindow*, double, double);

	void glfw_callback_click_(GLFWwindow*, int, int, int);

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};
}

// Function to process smooth movement that is frame rate independent
void processMovement(State_& state)
{
	for (auto const& [key, value] : state.keys)
	{
		if (value.first)
		{
			switch (key)
			{
				// Camera controls (for now it is always active)
				// Moving forward
			case GLFW_KEY_W:
				state.camera.position += state.camera.moveSpeed * state.deltaTime * state.camera.frontDirection;
				break;
				// Moving backwards
			case GLFW_KEY_S:
				state.camera.position -= state.camera.moveSpeed * state.deltaTime * state.camera.frontDirection;
				break;
				// Moving right
			case GLFW_KEY_D:
				state.camera.position += normalize(cross(state.camera.frontDirection, state.camera.upVector)) * state.camera.moveSpeed * state.deltaTime;
				break;
				// Moving left
			case GLFW_KEY_A:
				state.camera.position -= normalize(cross(state.camera.frontDirection, state.camera.upVector)) * state.camera.moveSpeed * state.deltaTime;
				break;
			}
		}
	}
}

void renderScene(State_& state, float dt, float fbwidth, float fbheight, bool secondScreen)
{
	// Local copies of camera variables so we don't change them across the two different view ports
	CameraState cameraState = (secondScreen ? state.camera.cameraStateSecondary : state.camera.cameraStateMain);
	Vec3f cameraPos = state.camera.position;
	Vec3f frontDirection = state.camera.frontDirection;

	// Adjust camera variables based on which camera is active
	if (cameraState == FIXED_DISTANCE)
	{
		cameraPos = spaceshipPos + state.rocketPosDelta + Vec3f{ 0.f, -2.f, 10.f };
		frontDirection = Vec3f{ 0.f, 0.f, -1.f };
	}
	else if (cameraState == GROUND_CAM)
	{
		cameraPos = spaceshipPos + Vec3f{ 4.f, -2.f, 38.f };
		frontDirection = normalize((spaceshipPos + state.rocketPosDelta) - cameraPos);
	}

	// Load model, view and projection matrices
	Mat44f modelMatrix = kIdentity44f;
	Mat44f viewMatrix = look_at(cameraPos, cameraPos + frontDirection, state.camera.upVector);
	Mat44f projection = make_perspective_projection(60.0f * PI / 180.0f, (state.splitScreen ? fbwidth/2 : fbwidth) / fbheight, 0.1f, 200.0f);

	// Normal matrix calculated from modelMatrix
	Mat33f normalMatrix = mat44_to_mat33(transpose(invert(modelMatrix)));

	// All matrices combined
	Mat44f mvpMatrix = projection * viewMatrix * modelMatrix;

	// Draw scene
	OGL_CHECKPOINT_DEBUG();

	// Check that if we are in split screen mode we don't clear the color buffer when drawing the 2nd screen
	if (state.splitScreen && secondScreen)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// Use main shader program and set uniforms
	glUseProgram(state.mainProgram->programId());

	glUniformMatrix4fv(0, 1, GL_TRUE, mvpMatrix.v);

	// Bind terrtain texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, state.terrainTextureID);

	// Bind the vertex array that has the vertices we want to draw
	glBindVertexArray(parlahtiVAO);

	// Draw said vertices
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei) parlahtiVertexCount);

	// Reset stuff
	glBindVertexArray(0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Use launchpad shader program and set uniforms
	glUseProgram(state.blinnPhongProgram->programId());

	// Light positions
	glUniform3fv(2, 3, &lightPositions[0][0]);
	// Light colors
	glUniform3fv(5, 3, &lightColors[0][0]);
	// View pos
	glUniform3fv(8, 1, &cameraPos.x);

	// Move first launchpad
	modelMatrix = modelMatrix * make_translation(Vec3f{ 5.f, -0.97f, -20.f });
	modelMatrix = modelMatrix * make_scaling(3.f, 3.f, 3.f);
	mvpMatrix = projection * viewMatrix * modelMatrix;
	normalMatrix = mat44_to_mat33(transpose(invert(modelMatrix)));

	glUniformMatrix4fv(0, 1, GL_TRUE, mvpMatrix.v);
	glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);

	glBindVertexArray(launchpadVAO);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei) launchpadVertexCount);

	// Move second launchpad
	modelMatrix = kIdentity44f * make_translation(Vec3f{ 40.f, -0.97f, 30.f });
	modelMatrix = modelMatrix * make_scaling(3.f, 3.f, 3.f);
	mvpMatrix = projection * viewMatrix * modelMatrix;

	glUniformMatrix4fv(0, 1, GL_TRUE, mvpMatrix.v);

	// Draw second launchpad
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei) launchpadVertexCount);

	// Setup spaceship
	modelMatrix = kIdentity44f * make_translation(spaceshipPos);

	// Check if the animation is active
	if (state.animationActive)
	{
		// Calculate angles and speed change for rocket
		state.animationActiveFor += dt;
		state.rocketPosDelta += Vec3f{ dt * state.animationActiveFor * 0.05f, min(dt * state.animationActiveFor * 0.5f, 5.f), 0.f };

		// Change position and rotation over time for a slight curved path
		modelMatrix = modelMatrix * make_translation(state.rocketPosDelta) * make_rotation_z(-state.animationActiveFor * 0.5f * PI / 180.f);
	}

	mvpMatrix = projection * viewMatrix * modelMatrix;
	normalMatrix = mat44_to_mat33(transpose(invert(modelMatrix)));

	glUniformMatrix4fv(0, 1, GL_TRUE, mvpMatrix.v);
	glUniformMatrix3fv(1, 1, GL_TRUE, normalMatrix.v);

	// Bind and draw spaceship VAO
	glBindVertexArray(spaceshipVAO);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei) spaceshipVertexCount);

	// Check if rocket is flying so we can render particles
	if (state.animationActive)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glUseProgram(state.particlesProgram->programId());

		// Pass in matrices seperately and calculate mvpMatrix within shader instead this time
		// since we need to use the model matrix on one of the VAO attributes seperately in the shader
		// before calculating the gl_Position
		glUniformMatrix4fv(0, 1, GL_TRUE, modelMatrix.v);
		glUniformMatrix4fv(1, 1, GL_TRUE, viewMatrix.v);
		glUniformMatrix4fv(2, 1, GL_TRUE, projection.v);
		glUniform3fv(3, 1, &cameraPos.x);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, state.particleTextureID);

		// Enable GL_PROGRAM_POINT_SIZE so we can use gl_PointSize in the vertex shader
		glEnable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(state.generator.vao);
		// Make sure to draw with GL_POINTS
		glDrawArrays(GL_POINTS, 0, (GLsizei) state.generator.particles.positions.size());

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glBindVertexArray(0);
	glUseProgram(0);

	OGL_CHECKPOINT_DEBUG();
}

int main() try
{
	// Initialize GLFW
	if( GLFW_TRUE != glfwInit() )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwInit() failed with '%s' (%d)", msg, ecode );
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_ );

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint( GLFW_DEPTH_BITS, 24 );

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if( !window )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwCreateWindow() failed with '%s' (%d)", msg, ecode );
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	State_ state{};
	glfwSetWindowUserPointer(window, &state);

	glfwSetKeyCallback( window, &glfw_callback_key_ );
	glfwSetCursorPosCallback(window, &glfw_callback_motion_);
	glfwSetMouseButtonCallback(window, &glfw_callback_click_);

	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if( !gladLoadGLLoader( (GLADloadproc)&glfwGetProcAddress ) )
		throw Error( "gladLoaDGLLoader() failed - cannot load GL API!" );

	std::printf( "RENDERER %s\n", glGetString( GL_RENDERER ) );
	std::printf( "VENDOR %s\n", glGetString( GL_VENDOR ) );
	std::printf( "VERSION %s\n", glGetString( GL_VERSION ) );
	std::printf( "SHADING_LANGUAGE_VERSION %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// Global GL setup
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	// This can be different from the window size, as standard window
	// decorations (title bar, borders, ...) may be included in the window size
	// but not be part of the drawable surface area.
	int iwidth, iheight;
	glfwGetFramebufferSize( window, &iwidth, &iheight );

	// Save width and height for use outside of the main function
	state.windowWidth = (float) iwidth;
	state.windowHeight = (float) iheight;

	// Main viewport
	glViewport( 0, 0, iwidth, iheight );

	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();
	
	// Further setup
	state.splitScreen = false;

	// Setup shaders
	ShaderProgram mainProgram({
		{ GL_VERTEX_SHADER, "assets/default.vert" },
		{ GL_FRAGMENT_SHADER, "assets/default.frag" }
	});

	ShaderProgram blinnPhongLighting({
		{ GL_VERTEX_SHADER, "assets/blinn-phong.vert" },
		{ GL_FRAGMENT_SHADER, "assets/blinn-phong.frag" }
	});

	ShaderProgram pointSprites({
		{ GL_VERTEX_SHADER, "assets/point-sprites.vert" },
		{ GL_FRAGMENT_SHADER, "assets/point-sprites.frag" }
	});

	ShaderProgram rectProgram({
		{ GL_VERTEX_SHADER, "assets/rect.vert" },
		{ GL_FRAGMENT_SHADER, "assets/rect.frag" }
	});

	ShaderProgram textProgram({
		{ GL_VERTEX_SHADER, "assets/text.vert" },
		{ GL_FRAGMENT_SHADER, "assets/text.frag" }
	});

	state.mainProgram = &mainProgram;
	state.blinnPhongProgram = &blinnPhongLighting;
	state.particlesProgram = &pointSprites;
	state.rectProgram = &rectProgram;
	state.textProgram = &textProgram;

	// Setup camera values
	
	// Start with free cam state
	state.camera.cameraStateMain = FREE_CAM;
	state.camera.cameraStateSecondary = FREE_CAM;

	// Camera position 2 units up and 5 units back
	state.camera.position = Vec3f{ 0.f, 2.f, 5.f };
	// Front direction in -z which is forward
	state.camera.frontDirection = Vec3f{ 0.f, 0.f, -1.f };
	// Up vector pointing straight up
	state.camera.upVector = Vec3f{ 0.f, 1.f, 0.f };

	// Constants for looking around
	state.camera.mouseSensitivity = 0.1f;
	state.camera.moveSpeed = 7.5f;
	state.camera.yaw = -90.f;
	state.camera.pitch = 0.f;
	state.camera.firstMouse = true;

	auto last = Clock::now();

	// Make the VAOs after all the OpenGL stuff is set up
	makeVAOs();
	GLuint rectangle = makeRectangle();
	GLuint rectangleLines = makeLines();
	Vec4f black = { 0.f, 0.f, 0.f, 1.0f };

	// Load terrain texture
	state.terrainTextureID = loadTexture2D("assets/L4343A-4k.jpeg");
	// Load particle texture
	state.particleTextureID = loadTexture2D("assets/white.png");

	// Setup ParticleGenerator
	state.generator.createVAO();

	// Setup fontstash
	state.fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);

	state.font = fonsAddFont(state.fs, "sans", "assets/DroidSansMonoDotted.ttf");
	if (state.font == FONS_INVALID)
	{
		throw Error("Could not add DroidSansMonoDotted.ttf font.\n");
	}

	state.buttonOneColor = Vec4f{ 0.8f, 0.8f, 0.8f, 0.2f };
	state.buttonTwoColor = Vec4f{ 0.8f, 0.8f, 0.8f, 0.2f };

	OGL_CHECKPOINT_ALWAYS();

	// Main loop
	while( !glfwWindowShouldClose( window ) )
	{
		// Let GLFW process events
		glfwPollEvents();
		
		///////////
		// SCENE //
		///////////
		
		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize( window, &nwidth, &nheight );

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if( 0 == nwidth || 0 == nheight )
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize( window, &nwidth, &nheight );
				} while( 0 == nwidth || 0 == nheight );
			}

			// Check splitscreen state to determine viewport
			if (state.splitScreen)
			{
				glViewport(0, 0, nwidth / 2, nheight);
			}
			else
			{
				glViewport(0, 0, nwidth, nheight);
			}
		}

		// Update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
		state.deltaTime = dt;
		last = now;

		if (state.animationActive) state.generator.update(dt, spaceshipPos + Vec3f{ -5.f, -9.2f, 20.f });

		// Render scene
		renderScene(state, dt, fbwidth, fbheight, false);

		// Render a 2nd screen if split screen is enabled
		if (state.splitScreen)
		{
			// Check if window was resized.
			{
				int nwidth, nheight;
				glfwGetFramebufferSize(window, &nwidth, &nheight);

				fbwidth = float(nwidth);
				fbheight = float(nheight);

				if (0 == nwidth || 0 == nheight)
				{
					// Window minimized? Pause until it is unminimized.
					// This is a bit of a hack.
					do
					{
						glfwWaitEvents();
						glfwGetFramebufferSize(window, &nwidth, &nheight);
					} while (0 == nwidth || 0 == nheight);
				}

				glViewport(nwidth/2, 0, nwidth/2, nheight);
			}
			renderScene(state, dt, fbwidth, fbheight, true);
		}

		/////////////////
		// UI ELEMENTS //
		/////////////////

		// Make a new viewport for the orthographic projection (for UI elements), to be
		// independent from split screen changes

		// Check if window was resized.
		{
			int nwidth, nheight;
			glfwGetFramebufferSize(window, &nwidth, &nheight);

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if (0 == nwidth || 0 == nheight)
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize(window, &nwidth, &nheight);
				} while (0 == nwidth || 0 == nheight);
			}

			glViewport(0, 0, nwidth, nheight);

			// Update the state window dimensions here since they are needed to calculate if
			// a button is being hovered or pressed
			state.windowWidth = (float) nwidth;
			state.windowHeight = (float) nheight;
		}

		// Here we use an orthographic projection for UI elements
		Mat44f orthographicMatrix = make_orthographic_projection(0.f, fbwidth, fbheight, 0.f);
		Mat44f modelMatrix = kIdentity44f;

		// Draw 2 buttons with rectangles
		glUseProgram(state.rectProgram->programId());

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		// Translate pos of button 1
		modelMatrix = modelMatrix * make_translation(Vec3f{ -0.15f, -0.9f, 0.f }) * make_scaling(0.1f, 0.05f, 1.0f);

		glUniformMatrix4fv(0, 1, GL_TRUE, modelMatrix.v);
		glUniform4fv(1, 1, &state.buttonOneColor.x);

		glBindVertexArray(rectangle);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glUniform4fv(1, 1, &black.x);
		glLineWidth(2.f);
		glBindVertexArray(rectangleLines);
		glDrawArrays(GL_LINES, 0, 8);

		// Translate pos of button 2
		modelMatrix = kIdentity44f * make_translation(Vec3f{ 0.15f, -0.9f, 0.f }) * make_scaling(0.1f, 0.05f, 1.0f);

		// Re send uniforms
		glUniformMatrix4fv(0, 1, GL_TRUE, modelMatrix.v);
		glUniform4fv(1, 1, &state.buttonTwoColor.x);

		glBindVertexArray(rectangle);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glUniform4fv(1, 1, &black.x);
		glLineWidth(2.f);
		glBindVertexArray(rectangleLines);
		glDrawArrays(GL_LINES, 0, 8);

		glBindVertexArray(0);

		glUseProgram(0);

		// Draw text for altitude and buttons
		glUseProgram(state.textProgram->programId());

		// Move model matrix to show text above buttons
		modelMatrix = kIdentity44f * make_translation(Vec3f{ 0.f, 0.f, 1.f });

		glUniformMatrix4fv(0, 1, GL_TRUE, orthographicMatrix.v);
		glUniformMatrix4fv(1, 1, GL_TRUE, modelMatrix.v);

		// Setup font variables
		int white = glfonsRGBA(255, 255, 255, 255);
		float dx = 0.f, dy = 0.f;
		float lineHeight = 0.f;

		// Draw altitude text
		fonsClearState(state.fs);
		fonsSetFont(state.fs, state.font);
		fonsSetSize(state.fs, 32.f);
		fonsSetAlign(state.fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
		fonsVertMetrics(state.fs, NULL, NULL, &lineHeight);
		fonsSetColor(state.fs, white);
		char altitude[10];
		std::snprintf(altitude, 10, "%f", state.rocketPosDelta.y);
		dx += fonsDrawText(state.fs, dx, dy, "Altitude: ", NULL);
		fonsDrawText(state.fs, dx, dy, altitude, NULL);

		// Draw button text
		fonsSetSize(state.fs, 24.f);
		fonsSetAlign(state.fs, FONS_ALIGN_CENTER);
		fonsVertMetrics(state.fs, NULL, NULL, &lineHeight);
		fonsSetColor(state.fs, glfonsRGBA(0, 0, 0, 255));
		fonsDrawText(state.fs, (state.windowWidth / 2.f) - (state.windowWidth * 0.075f), (state.windowHeight * 0.96f), "Liftoff!", NULL);
		fonsDrawText(state.fs, (state.windowWidth / 2.f) + (state.windowWidth * 0.075f), (state.windowHeight * 0.96f), "Reset", NULL);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);

		glUseProgram(0);
		
		// Display results
		glfwSwapBuffers( window );

		processMovement(state);
	}

	// Cleanup.
	state.mainProgram = nullptr;
	
	return 0;
}
catch( std::exception const& eErr )
{
	std::fprintf( stderr, "Top-level Exception (%s):\n", typeid(eErr).name() );
	std::fprintf( stderr, "%s\n", eErr.what() );
	std::fprintf( stderr, "Bye.\n" );
	return 1;
}


namespace
{
	void glfw_callback_error_( int aErrNum, char const* aErrDesc )
	{
		std::fprintf( stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum );
	}

	void glfw_callback_key_( GLFWwindow* aWindow, int aKey, int, int aAction, int mods )
	{
		if( GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction )
		{
			glfwSetWindowShouldClose( aWindow, GLFW_TRUE );
			return;
		}

		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			// R-key reloads shaders and resets rocket animation
			if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
			{
				if (state->mainProgram)
				{
					try
					{
						state->mainProgram->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}

					try
					{
						state->blinnPhongProgram->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}

					try
					{
						state->particlesProgram->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}

					try
					{
						state->textProgram->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}

					try
					{
						state->rectProgram->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}
				}

				state->generator.resetParticles();
			}

			// Enable / disable camera
			if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
			{
				state->camera.active = !state->camera.active;

				if (state->camera.active)
				{
					// Disable cursor so we can control the camera like
					// a proper first person camera
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				else
				{
					// Reenable the cursor to be able to move off the window
					// or click buttons
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
			}

			// Log key presses and releases in order to process smooth movement
			if (GLFW_PRESS == aAction)
			{
				state->keys[aKey] = std::pair(true, mods);
			}
			else if (GLFW_RELEASE == aAction)
			{
				state->keys[aKey] = std::pair(false, mods);
			}

			// Camera speed modifiers
			if (mods == GLFW_MOD_SHIFT)
			{
				state->camera.moveSpeed = 15.f;
			}
			else if (mods == GLFW_MOD_CONTROL)
			{
				state->camera.moveSpeed = 3.75f;
			}
			else
			{
				state->camera.moveSpeed = 7.5f;
			}

			// Start rocket animation
			if (GLFW_KEY_F == aKey)
			{
				state->animationActive = true;
				state->rocketPosDelta = Vec3f{ 0.f, 0.f, 0.f };
				state->animationActiveFor = 0.1f;
			}
			// Reset rocket animation
			if (GLFW_KEY_R == aKey)
			{
				state->animationActive = false;
				state->rocketPosDelta = Vec3f{ 0.f, 0.f, 0.f };
				state->animationActiveFor = 0.1f;
			}

			// Cycle through camera main states (C)
			if (GLFW_KEY_C == aKey && GLFW_PRESS == aAction && GLFW_MOD_SHIFT != mods)
			{
				switch (state->camera.cameraStateMain)
				{
				case FREE_CAM:
					state->camera.cameraStateMain = FIXED_DISTANCE;
					break;
				case FIXED_DISTANCE:
					state->camera.cameraStateMain = GROUND_CAM;
					break;
				case GROUND_CAM:
					state->camera.cameraStateMain = FREE_CAM;
					break;
				}
			}

			// Cycle through camera secondary states (SHIFT + C)
			if (GLFW_KEY_C == aKey && GLFW_PRESS == aAction && GLFW_MOD_SHIFT == mods)
			{
				switch (state->camera.cameraStateSecondary)
				{
				case FREE_CAM:
					state->camera.cameraStateSecondary = FIXED_DISTANCE;
					break;
				case FIXED_DISTANCE:
					state->camera.cameraStateSecondary = GROUND_CAM;
					break;
				case GROUND_CAM:
					state->camera.cameraStateSecondary = FREE_CAM;
					break;
				}
			}

			// Toggle split screen
			if (GLFW_KEY_V == aKey && GLFW_PRESS == aAction)
			{
				state->splitScreen = !state->splitScreen;
			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camera.active)
			{
				// Set lastX and lastY on first mouse callback so the camera doesnt jump
				if (state->camera.firstMouse)
				{
					state->camera.lastX = (float) aX;
					state->camera.lastY = (float) aY;
					state->camera.firstMouse = false;
				}

				// Calculate offsets based on how much the mouse moved
				float xOffset = (float) aX - state->camera.lastX;
				float yOffset = state->camera.lastY - (float) aY;
				state->camera.lastX = (float) aX;
				state->camera.lastY = (float) aY;

				// Adjust with mouse sensitivity
				xOffset *= state->camera.mouseSensitivity;
				yOffset *= state->camera.mouseSensitivity;

				state->camera.yaw += xOffset;
				state->camera.pitch += yOffset;

				// Restrict pitch and yaw so we dont flip the camera
				if (state->camera.pitch > 89.f)
					state->camera.pitch = 89.f;
				if (state->camera.pitch < -89.f)
					state->camera.pitch = -89.f;

				// Calculate new front vector and update frontDirection
				Vec3f frontVector;
				frontVector.x = cos(state->camera.yaw * (PI / 180.f)) * cos(state->camera.pitch * (PI / 180.f));
				frontVector.y = sin(state->camera.pitch * (PI / 180.f));
				frontVector.z = sin(state->camera.yaw * (PI / 180.f)) * cos(state->camera.pitch * (PI / 180.f));
				state->camera.frontDirection = normalize(frontVector);
			}
			else
			{
				// Button 1 checks
				if (aX >= (state->windowWidth / 2) - (state->windowWidth * 0.075f) - (state->windowWidth * 0.05f) &&
					aX <= (state->windowWidth / 2) - (state->windowWidth * 0.025f) &&
					aY >= (state->windowHeight * 0.9f) + (state->windowHeight * 0.025f) &&
					aY <= (state->windowHeight * 0.95f) + (state->windowHeight * 0.025f))
				{
					state->buttonOneColor = Vec4f{ 0.f, 1.f, 0.f, 0.2f };
				}
				else
				{
					state->buttonOneColor = Vec4f{ 0.8f, 0.8f, 0.8f, 0.2f };
				}

				// Button 2 checks
				if (aX <= (state->windowWidth / 2) + (state->windowWidth * 0.075f) + (state->windowWidth * 0.05f) &&
					aX >= (state->windowWidth / 2) + (state->windowWidth * 0.025f) &&
					aY >= (state->windowHeight * 0.9f) + (state->windowHeight * 0.025f) &&
					aY <= (state->windowHeight * 0.95f) + (state->windowHeight * 0.025f))
				{
					state->buttonTwoColor = Vec4f{ 0.f, 1.f, 0.f, 0.2f };
				}
				else
				{
					state->buttonTwoColor = Vec4f{ 0.8f, 0.8f, 0.8f, 0.2f };
				}
			}
		}
	}

	void glfw_callback_click_(GLFWwindow* aWindow, int button, int action, int)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			// Get cursor position
			double aX, aY;
			glfwGetCursorPos(aWindow, &aX, &aY);

			if (!state->camera.active)
			{
				if (GLFW_MOUSE_BUTTON_LEFT == button && GLFW_PRESS == action)
				{
					// Button 1 checks
					if (aX >= (state->windowWidth / 2) - (state->windowWidth * 0.075f) - (state->windowWidth * 0.05f) &&
						aX <= (state->windowWidth / 2) - (state->windowWidth * 0.025f) &&
						aY >= (state->windowHeight * 0.9f) + (state->windowHeight * 0.025f) &&
						aY <= (state->windowHeight * 0.95f) + (state->windowHeight * 0.025f))
					{
						state->buttonOneColor = Vec4f{ 1.f, 0.f, 0.f, 0.2f };

						// Start rocket animation
						state->animationActive = true;
						state->rocketPosDelta = Vec3f{ 0.f, 0.f, 0.f };
						state->animationActiveFor = 0.1f;
					}

					// Button 2 checks
					if (aX <= (state->windowWidth / 2) + (state->windowWidth * 0.075f) + (state->windowWidth * 0.05f) &&
						aX >= (state->windowWidth / 2) + (state->windowWidth * 0.025f) &&
						aY >= (state->windowHeight * 0.9f) + (state->windowHeight * 0.025f) &&
						aY <= (state->windowHeight * 0.95f) + (state->windowHeight * 0.025f))
					{
						state->buttonTwoColor = Vec4f{ 1.f, 0.f, 0.f, 0.2f };

						// Reset rocket animation
						state->animationActive = false;
						state->rocketPosDelta = Vec3f{ 0.f, 0.f, 0.f };
						state->animationActiveFor = 0.1f;
					}
				}
				
			}
		}
	}
}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if( window )
			glfwDestroyWindow( window );
	}
}

