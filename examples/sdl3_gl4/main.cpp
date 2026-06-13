#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "abs/abs.hpp"

static PFNGLCREATESHADERPROC p_glCreateShader = nullptr;
static PFNGLSHADERSOURCEPROC p_glShaderSource = nullptr;
static PFNGLCOMPILESHADERPROC p_glCompileShader = nullptr;
static PFNGLGETSHADERIVPROC p_glGetShaderiv = nullptr;
static PFNGLGETSHADERINFOLOGPROC p_glGetShaderInfoLog = nullptr;
static PFNGLDELETESHADERPROC p_glDeleteShader = nullptr;
static PFNGLCREATEPROGRAMPROC p_glCreateProgram = nullptr;
static PFNGLATTACHSHADERPROC p_glAttachShader = nullptr;
static PFNGLLINKPROGRAMPROC p_glLinkProgram = nullptr;
static PFNGLGETPROGRAMIVPROC p_glGetProgramiv = nullptr;
static PFNGLGETPROGRAMINFOLOGPROC p_glGetProgramInfoLog = nullptr;
static PFNGLDELETEPROGRAMPROC p_glDeleteProgram = nullptr;
static PFNGLUSEPROGRAMPROC p_glUseProgram = nullptr;
static PFNGLGENVERTEXARRAYSPROC p_glGenVertexArrays = nullptr;
static PFNGLBINDVERTEXARRAYPROC p_glBindVertexArray = nullptr;
static PFNGLDELETEVERTEXARRAYSPROC p_glDeleteVertexArrays = nullptr;
static PFNGLGENBUFFERSPROC p_glGenBuffers = nullptr;
static PFNGLBINDBUFFERPROC p_glBindBuffer = nullptr;
static PFNGLBUFFERDATAPROC p_glBufferData = nullptr;
static PFNGLDELETEBUFFERSPROC p_glDeleteBuffers = nullptr;
static PFNGLENABLEVERTEXATTRIBARRAYPROC p_glEnableVertexAttribArray = nullptr;
static PFNGLVERTEXATTRIBPOINTERPROC p_glVertexAttribPointer = nullptr;
static PFNGLGETUNIFORMLOCATIONPROC p_glGetUniformLocation = nullptr;
static PFNGLUNIFORMMATRIX4FVPROC p_glUniformMatrix4fv = nullptr;
static PFNGLUNIFORM3FPROC p_glUniform3f = nullptr;

static SDL_FunctionPointer load_gl(const char* name) {
    return SDL_GL_GetProcAddress(name);
}

static bool load_gl_functions() {
    p_glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(load_gl("glCreateShader"));
    p_glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(load_gl("glShaderSource"));
    p_glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(load_gl("glCompileShader"));
    p_glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(load_gl("glGetShaderiv"));
    p_glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(load_gl("glGetShaderInfoLog"));
    p_glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(load_gl("glDeleteShader"));
    p_glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(load_gl("glCreateProgram"));
    p_glAttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(load_gl("glAttachShader"));
    p_glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(load_gl("glLinkProgram"));
    p_glGetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(load_gl("glGetProgramiv"));
    p_glGetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(load_gl("glGetProgramInfoLog"));
    p_glDeleteProgram = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(load_gl("glDeleteProgram"));
    p_glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(load_gl("glUseProgram"));
    p_glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(load_gl("glGenVertexArrays"));
    p_glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(load_gl("glBindVertexArray"));
    p_glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(load_gl("glDeleteVertexArrays"));
    p_glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(load_gl("glGenBuffers"));
    p_glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(load_gl("glBindBuffer"));
    p_glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(load_gl("glBufferData"));
    p_glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(load_gl("glDeleteBuffers"));
    p_glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(load_gl("glEnableVertexAttribArray"));
    p_glVertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(load_gl("glVertexAttribPointer"));
    p_glGetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(load_gl("glGetUniformLocation"));
    p_glUniformMatrix4fv = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(load_gl("glUniformMatrix4fv"));
    p_glUniform3f = reinterpret_cast<PFNGLUNIFORM3FPROC>(load_gl("glUniform3f"));

    return p_glCreateShader && p_glShaderSource && p_glCompileShader && p_glGetShaderiv && p_glGetShaderInfoLog &&
           p_glDeleteShader && p_glCreateProgram && p_glAttachShader && p_glLinkProgram && p_glGetProgramiv &&
           p_glGetProgramInfoLog && p_glDeleteProgram && p_glUseProgram && p_glGenVertexArrays &&
           p_glBindVertexArray && p_glDeleteVertexArrays && p_glGenBuffers && p_glBindBuffer && p_glBufferData &&
           p_glDeleteBuffers && p_glEnableVertexAttribArray && p_glVertexAttribPointer &&
           p_glGetUniformLocation && p_glUniformMatrix4fv && p_glUniform3f;
}

static bool check_shader(GLuint shader, const char* label) {
    GLint ok = 0;
    p_glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok == GL_TRUE) {
        return true;
    }

    GLchar log[1024];
    GLsizei len = 0;
    p_glGetShaderInfoLog(shader, sizeof(log), &len, log);
    std::fprintf(stderr, "%s compile failed: %.*s\n", label, static_cast<int>(len), log);
    return false;
}

static bool check_program(GLuint program) {
    GLint ok = 0;
    p_glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (ok == GL_TRUE) {
        return true;
    }

    GLchar log[1024];
    GLsizei len = 0;
    p_glGetProgramInfoLog(program, sizeof(log), &len, log);
    std::fprintf(stderr, "program link failed: %.*s\n", static_cast<int>(len), log);
    return false;
}

static GLuint build_program() {
    const char* vertex_src =
        "#version 410 core\n"
        "layout(location=0) in vec3 a_pos;\n"
        "uniform mat4 u_mvp;\n"
        "void main() {\n"
        "    gl_Position = u_mvp * vec4(a_pos, 1.0);\n"
        "}\n";

    const char* fragment_src =
        "#version 410 core\n"
        "out vec4 frag_color;\n"
        "uniform vec3 u_color;\n"
        "void main() {\n"
        "    frag_color = vec4(u_color, 1.0);\n"
        "}\n";

    const GLuint vs = p_glCreateShader(GL_VERTEX_SHADER);
    p_glShaderSource(vs, 1, &vertex_src, nullptr);
    p_glCompileShader(vs);
    if (!check_shader(vs, "vertex")) {
        p_glDeleteShader(vs);
        return 0;
    }

    const GLuint fs = p_glCreateShader(GL_FRAGMENT_SHADER);
    p_glShaderSource(fs, 1, &fragment_src, nullptr);
    p_glCompileShader(fs);
    if (!check_shader(fs, "fragment")) {
        p_glDeleteShader(vs);
        p_glDeleteShader(fs);
        return 0;
    }

    const GLuint program = p_glCreateProgram();
    p_glAttachShader(program, vs);
    p_glAttachShader(program, fs);
    p_glLinkProgram(program);

    p_glDeleteShader(vs);
    p_glDeleteShader(fs);

    if (!check_program(program)) {
        p_glDeleteProgram(program);
        return 0;
    }

    return program;
}

static void mat4_to_gl_column_major(const mat4& m, float out[16]) {
    out[0] = m.m[0][0];
    out[1] = m.m[1][0];
    out[2] = m.m[2][0];
    out[3] = m.m[3][0];

    out[4] = m.m[0][1];
    out[5] = m.m[1][1];
    out[6] = m.m[2][1];
    out[7] = m.m[3][1];

    out[8] = m.m[0][2];
    out[9] = m.m[1][2];
    out[10] = m.m[2][2];
    out[11] = m.m[3][2];

    out[12] = m.m[0][3];
    out[13] = m.m[1][3];
    out[14] = m.m[2][3];
    out[15] = m.m[3][3];
}

struct orbit_camera {
    vec3 target;
    vec3 up;
    float yaw;
    float pitch;
    float radius;
    camera_perspective camera;
};

static vec3 orbit_camera_position(const orbit_camera& c) {
    const float cp = std::cos(c.pitch);
    const vec3 offset{
        c.radius * cp * std::sin(c.yaw),
        c.radius * std::sin(c.pitch),
        c.radius * cp * std::cos(c.yaw),
    };
    return c.target + offset;
}

static camera_perspective orbit_camera_eval(const orbit_camera& c) {
    camera_perspective cam = c.camera;
    cam.position = orbit_camera_position(c);
    cam.target = c.target;
    cam.up = c.up;
    return cam;
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow("abs SDL3 OpenGL4 Example", 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        std::fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!load_gl_functions()) {
        std::fprintf(stderr, "OpenGL function load failed; check OpenGL 4+ driver support.\n");
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1);

    const GLuint program = build_program();
    if (!program) {
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const GLint u_mvp = p_glGetUniformLocation(program, "u_mvp");
    const GLint u_color = p_glGetUniformLocation(program, "u_color");

    const float vertices[] = {
        0.0f, 0.7f, 0.0f,
        -0.6f, -0.5f, 0.0f,
        0.6f, -0.5f, 0.0f,
    };

    GLuint vao = 0;
    GLuint vbo = 0;
    p_glGenVertexArrays(1, &vao);
    p_glBindVertexArray(vao);

    p_glGenBuffers(1, &vbo);
    p_glBindBuffer(GL_ARRAY_BUFFER, vbo);
    p_glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    p_glEnableVertexAttribArray(0);
    p_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<const void*>(0));

    camera_perspective base_camera = camera_perspective_default();
    base_camera.fov_y_radians = 1.0471975512f;
    base_camera.z_near = 0.1f;
    base_camera.z_far = 100.0f;
    base_camera.h = HANDEDNESS_RIGHT;

    orbit_camera camera{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 0.0f, 0.0f, 2.2f, base_camera};

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                running = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_A:
                        camera.yaw -= 0.08f;
                        break;
                    case SDLK_D:
                        camera.yaw += 0.08f;
                        break;
                    case SDLK_W:
                        camera.pitch = clampf(camera.pitch + 0.06f, -1.35f, 1.35f);
                        break;
                    case SDLK_S:
                        camera.pitch = clampf(camera.pitch - 0.06f, -1.35f, 1.35f);
                        break;
                    case SDLK_Q:
                        camera.radius = maxf(0.7f, camera.radius - 0.12f);
                        break;
                    case SDLK_E:
                        camera.radius += 0.12f;
                        break;
                    default:
                        break;
                }
            }
        }

        int width = 0;
        int height = 0;
        SDL_GetWindowSize(window, &width, &height);
        if (height <= 0) {
            height = 1;
        }

        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float t = static_cast<float>(SDL_GetTicks()) * 0.001f;
        const quat rot = quat_from_axis_angle({0.0f, 1.0f, 0.0f}, t);
        const mat4 model = mat4_trs({0.0f, 0.0f, 0.0f}, rot, {1.0f, 1.0f, 1.0f});
        const camera_perspective active_camera = orbit_camera_eval(camera);
        const mat4 view = camera_view_matrix(active_camera);
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        const mat4 proj = camera_projection_matrix(active_camera, aspect);
        const mat4 mvp = mat4_mul(proj, mat4_mul(view, model));

        float mvp_gl[16];
        mat4_to_gl_column_major(mvp, mvp_gl);

        p_glUseProgram(program);
        p_glUniformMatrix4fv(u_mvp, 1, GL_FALSE, mvp_gl);
        p_glUniform3f(u_color, 0.25f, 0.78f, 0.98f);

        p_glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    p_glDeleteBuffers(1, &vbo);
    p_glDeleteVertexArrays(1, &vao);
    p_glDeleteProgram(program);

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
