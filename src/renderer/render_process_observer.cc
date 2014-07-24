// Copyright (c) 2014 Stanislas Polu.
// Copyright (c) 2012 The Chromium Authors.
// See the LICENSE file.

#include "src/renderer/render_process_observer.h"

#include "base/command_line.h"
#include "content/public/common/content_client.h"
#include "content/public/renderer/render_thread.h"
#include "content/public/renderer/render_view.h"
#include "content/public/test/layouttest_support.h"

#include "src/common/messages.h"
#include "src/common/switches.h"
#include "src/renderer/renderer_client.h"

using namespace content;

namespace exo_browser {

namespace {
ExoBrowserRenderProcessObserver* g_instance = NULL;
}

// static
ExoBrowserRenderProcessObserver* 
ExoBrowserRenderProcessObserver::GetInstance() 
{
  return g_instance;
}

ExoBrowserRenderProcessObserver::ExoBrowserRenderProcessObserver()
{
  CHECK(!g_instance);
  g_instance = this;
  RenderThread::Get()->AddObserver(this);
}

ExoBrowserRenderProcessObserver::~ExoBrowserRenderProcessObserver() 
{
  CHECK(g_instance == this);
  g_instance = NULL;
}

void 
ExoBrowserRenderProcessObserver::SetMainWindow(
    RenderView* view) 
{
  return;
}

void 
ExoBrowserRenderProcessObserver::WebKitInitialized() 
{
  return;
}

bool 
ExoBrowserRenderProcessObserver::OnControlMessageReceived(
    const IPC::Message& message) 
{
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(ExoBrowserRenderProcessObserver, message)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  return handled;
}

} // namespace exo_browser
