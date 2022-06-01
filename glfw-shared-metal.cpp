#include <GLFW/glfw3.h>
#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif
#ifdef __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#endif
#include <GLFW/glfw3native.h>

#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/SwapChain.h>
#include <filament/Scene.h>
#include <filament/View.h>
#include <filament/Viewport.h>
#include <filament/Camera.h>
#include <filament/TransformManager.h>
#include <filament/Texture.h>
#include <filament/TextureSampler.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>
#include <filament/RenderableManager.h>
#include <filament/Material.h>
#include <filameshio/MeshReader.h>
#include <math/mat3.h>
#include <math/mat4.h>
#include <utils/Path.h>
#include <utils/EntityManager.h>

#include "mac_helpers.h"
#include "metal_texture.h"

extern unsigned char bakedTexture_matc[];
extern unsigned int bakedTexture_matc_len;

using namespace filament;
using namespace utils;

int main(int argc, char *argv[])
{
  bool badmatch = false;
  if (argc > 1 && std::string(argv[1]) == "--badmatch")
  {
    badmatch = true;
  }

  if (!glfwInit())
  {
    printf("Error: cannot setup glfw.\n");
    return -1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow *win = glfwCreateWindow(400, 400, "Main", NULL, NULL);

  Engine *engine = Engine::create(
      backend::Backend::METAL);

  void *native_window = createMetalLayer(win);

  SwapChain *swap_chain = engine->createSwapChain(native_window);
  Renderer *renderer = engine->createRenderer();
  Scene *scene = engine->createScene();
  View *view = engine->createView();
  Entity c = EntityManager::get().create();
  Camera *cam = engine->createCamera(c);

  view->setScene(scene);
  view->setCamera(cam);

  renderer->setClearOptions({.clearColor = {0.0f, 0.0f, 0.0f, 1.0f},
                             .clear = true});

  struct Vertex
  {
    math::float2 position;
    math::float2 uv;
  };

  static const Vertex QUAD_VERTICES[4] = {
      {{-1, -1}, {0, 0}},
      {{1, -1}, {1, 0}},
      {{-1, 1}, {0, 1}},
      {{1, 1}, {1, 1}},
  };

  static constexpr uint16_t QUAD_INDICES[6] = {
      0,
      1,
      2,
      3,
      2,
      1,
  };

  Texture *texture = Texture::Builder()
                          .import((intptr_t) createTexture())
                          .sampler(Texture::Sampler::SAMPLER_2D)
                          .width(2)
                          .height(2)
                          .levels(1)
                          .build(*engine);

  TextureSampler sampler(TextureSampler::MinFilter::LINEAR, TextureSampler::MagFilter::LINEAR);
  auto vb = VertexBuffer::Builder()
                .vertexCount(4)
                .bufferCount(1)
                .attribute(VertexAttribute::POSITION, 0, VertexBuffer::AttributeType::FLOAT2, 0, 16)
                .attribute(VertexAttribute::UV0, 0, VertexBuffer::AttributeType::FLOAT2, 8, 16)
                .build(*engine);
  vb->setBufferAt(*engine, 0,
                  VertexBuffer::BufferDescriptor(QUAD_VERTICES, 64, nullptr));
  auto ib = IndexBuffer::Builder()
                .indexCount(6)
                .bufferType(IndexBuffer::IndexType::USHORT)
                .build(*engine);
  ib->setBuffer(*engine,
                IndexBuffer::BufferDescriptor(QUAD_INDICES, 12, nullptr));
  auto mat = Material::Builder()
                  .package(bakedTexture_matc, bakedTexture_matc_len)
                  .build(*engine);
  auto mat_inst = mat->createInstance();
  mat_inst->setParameter("albedo", texture, sampler);
  auto renderable = EntityManager::get().create();
  RenderableManager::Builder(1)
      .boundingBox({{-1, -1, -1}, {1, 1, 1}})
      .material(0, mat_inst)
      .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, vb, ib, 0, 6)
      .culling(false)
      .receiveShadows(false)
      .castShadows(false)
      .build(*engine, renderable);
  scene->addEntity(renderable);

  uint32_t i = 0;
  while (!glfwWindowShouldClose(win))
  {
    updateTexture();
    int w, h;
    glfwGetWindowSize(win, &w, &h);

    cam->setProjection(45.0f, float(w) / h, 0.1f, 100.0f);
    cam->lookAt({0, 0, 10.0}, {0, 0, 0}, {0, 1, 0});
    view->setViewport({0, 0, (uint32_t)w, (uint32_t)h});

    if (renderer->beginFrame(swap_chain))
    {
      renderer->render(view);
      renderer->endFrame();
    }

    glfwSwapBuffers(win);

    glfwPollEvents();
  }

  Engine::destroy(&engine);

  glfwTerminate();

  return 0;
}
