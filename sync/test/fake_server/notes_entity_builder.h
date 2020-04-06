// Copyright (c) 2015-2017 Vivaldi Technologies AS. All rights reserved
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VIVALDI_SYNC_TEST_FAKE_SERVER_NOTES_ENTITY_BUILDER_H_
#define VIVALDI_SYNC_TEST_FAKE_SERVER_NOTES_ENTITY_BUILDER_H_

#include <string>

#include "components/sync/base/model_type.h"
#include "components/sync/test/fake_server/fake_server_entity.h"
#include "url/gurl.h"

namespace fake_server {

// Builder for NotesEntity objects.
class NotesEntityBuilder {
 public:
  NotesEntityBuilder(const std::string& title,
                     const GURL& url,
                     const std::string& content,
                     const std::string& originator_cache_guid,
                     const std::string& originator_client_item_id);
  NotesEntityBuilder(const NotesEntityBuilder&);
  ~NotesEntityBuilder();

  // EntityBuilder
  std::unique_ptr<FakeServerEntity> Build();

 private:
  // The note's URL.
  GURL url_;
  const std::string title_;
  const std::string content_;
  std::string originator_cache_guid_;
  std::string originator_client_item_id_;
};

}  // namespace fake_server

#endif  // VIVALDI_SYNC_TEST_FAKE_SERVER_NOTES_ENTITY_BUILDER_H_