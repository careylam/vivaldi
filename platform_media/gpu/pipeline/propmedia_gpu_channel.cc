// Copyright (c) 2016 Vivaldi Technologies AS. All rights reserved.
// Copyright (C) 2014 Opera Software ASA.  All rights reserved.
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform_media/gpu/pipeline/propmedia_gpu_channel.h"

#if defined(USE_SYSTEM_PROPRIETARY_CODECS)

#include "platform_media/common/media_pipeline_messages.h"
#include "platform_media/gpu/pipeline/ipc_media_pipeline.h"

#include "gpu/command_buffer/service/gpu_preferences.h"
#include "gpu/command_buffer/service/preemption_flag.h"
#include "gpu/command_buffer/service/mailbox_manager.h"
#include "gpu/config/gpu_switches.h"
#include "gpu/ipc/common/gpu_messages.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "ipc/ipc_channel.h"
#include "ipc/message_filter.h"
#include "base/command_line.h"
#include "build/build_config.h"

namespace gpu {

ProprietaryMediaGpuChannel::ProprietaryMediaGpuChannel(
             GpuChannelManager* gpu_channel_manager,
             Scheduler* scheduler,
             SyncPointManager* sync_point_manager,
             GpuWatchdogThread* watchdog,
             scoped_refptr<gl::GLShareGroup> share_group,
             scoped_refptr<gles2::MailboxManager> mailbox_manager,
             ServiceDiscardableManager* discardable_manager,
             scoped_refptr<PreemptionFlag> preempting_flag,
             scoped_refptr<PreemptionFlag> preempted_flag,
             scoped_refptr<base::SingleThreadTaskRunner> task_runner,
             scoped_refptr<base::SingleThreadTaskRunner> io_task_runner,
             int32_t client_id,
             uint64_t client_tracing_id,
             bool is_gpu_host)
  : GpuChannel(
             gpu_channel_manager,
             scheduler,
             sync_point_manager,
             watchdog,
             share_group,
             mailbox_manager,
             discardable_manager,
             preempting_flag,
             preempted_flag,
             task_runner,
             io_task_runner,
             client_id,
             client_tracing_id,
             is_gpu_host) {}

ProprietaryMediaGpuChannel::~ProprietaryMediaGpuChannel() {}

bool ProprietaryMediaGpuChannel::OnControlMessageReceived(
      const IPC::Message& msg
    ) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(ProprietaryMediaGpuChannel, msg)
#if defined(USE_SYSTEM_PROPRIETARY_CODECS)
    IPC_MESSAGE_HANDLER(MediaPipelineMsg_New, OnNewMediaPipeline)
    IPC_MESSAGE_HANDLER(MediaPipelineMsg_Destroy, OnDestroyMediaPipeline)
#endif
    IPC_MESSAGE_UNHANDLED(handled = GpuChannel::OnControlMessageReceived(msg))
  IPC_END_MESSAGE_MAP()
  return handled;
}

void ProprietaryMediaGpuChannel::OnNewMediaPipeline(
    int32_t route_id,
    int32_t gpu_video_accelerator_factories_route_id) {
  GpuCommandBufferStub* command_buffer = nullptr;

  const base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
  if (!gpu_channel_manager()->gpu_preferences().disable_accelerated_video_decode &&
      cmd_line->HasSwitch(switches::kEnablePlatformAcceleratedVideoDecoding)) {
    command_buffer =
        LookupCommandBuffer(gpu_video_accelerator_factories_route_id);
  }

  std::unique_ptr<content::IPCMediaPipeline> ipc_media_pipeline(
      new content::IPCMediaPipeline(this, route_id, command_buffer));
  AddRoute(route_id, GetSequenceId(),
           ipc_media_pipeline.get());
  media_pipelines_.AddWithID(std::move(ipc_media_pipeline), route_id);
}

void ProprietaryMediaGpuChannel::OnDestroyMediaPipeline(int32_t route_id) {
  RemoveRoute(route_id);
  media_pipelines_.Remove(route_id);
}

}  // namespace gpu

#endif
