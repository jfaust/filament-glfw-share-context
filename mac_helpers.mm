#include "mac_helpers.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#include <GLFW/glfw3native.h>

#import <QuartzCore/QuartzCore.h>

void* getContentView(GLFWwindow* win) {
  NSWindow* window = (NSWindow*)glfwGetCocoaWindow(win);
  NSView* content_view = window.contentView;
  return content_view;
}

void* createMetalLayer(GLFWwindow* win) {
  NSWindow* window = (NSWindow*)glfwGetCocoaWindow(win);
  NSView* content_view = window.contentView;
  [content_view setLayer: [CAMetalLayer layer]];
  return content_view.layer;
}