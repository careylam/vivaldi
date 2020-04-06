//
// Copyright (c) 2016 Vivaldi Technologies AS. All rights reserved.
//

#ifndef UI_VIVALDI_SESSION_SERVICE_H_
#define UI_VIVALDI_SESSION_SERVICE_H_

#include <map>
#include <utility>
#include <string>
#include <vector>
#include "base/files/file.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_util.h"
#include "base/memory/linked_ptr.h"
#include "base/memory/scoped_vector.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/sessions/session_restore.h"
#include "chrome/browser/sessions/session_restore_delegate.h"
#include "chrome/browser/sessions/session_service.h"
#include "chrome/browser/sessions/session_service_utils.h"
#include "chrome/browser/sessions/session_tab_helper.h"
#include "components/sessions/core/session_command.h"
#include "components/sessions/core/session_constants.h"
#include "components/sessions/core/session_service_commands.h"

namespace vivaldi {

// We cannot inherit from SessionService, and changing that class to be
// suitable is risky and requires significant changes.
class VivaldiSessionService {
 public:
  typedef std::map<SessionID::id_type, std::pair<int, int> > IdToRange;
  typedef sessions::SessionCommand::id_type id_type;
  typedef sessions::SessionCommand::size_type size_type;

  explicit VivaldiSessionService(Profile* profile);
  VivaldiSessionService();
  ~VivaldiSessionService();

  bool ShouldTrackWindow(Browser* browser, Profile* profile);
  void ScheduleCommand(std::unique_ptr<sessions::SessionCommand> command);
  void BuildCommandsForTab(const SessionID& window_id,
                          content::WebContents* tab, int index_in_window,
                          bool is_pinned);
  void BuildCommandsForBrowser(Browser* browser);
  bool Save(const base::FilePath& file_name);
  bool Load(const base::FilePath& file_name, Browser* browser);

 private:
  void ResetFile(const base::FilePath& file_name);
  base::File* OpenAndWriteHeader(const base::FilePath& path);
  bool AppendCommandsToFile(
      base::File* file,
      const ScopedVector<sessions::SessionCommand>& commands);
  bool Read(ScopedVector<sessions::SessionCommand>* commands);
  bool FillBuffer();
  Browser* ProcessSessionWindows(
      std::vector<sessions::SessionWindow*>* windows,
      SessionID::id_type active_window_id,
      std::vector<SessionRestoreDelegate::RestoredTab>* created_contents);
  void RestoreTabsToBrowser(
      const sessions::SessionWindow& window, Browser* browser,
      int initial_tab_count, int selected_tab_index,
      std::vector<SessionRestoreDelegate::RestoredTab>* created_contents);
  void RemoveUnusedRestoreWindows(
      std::vector<sessions::SessionWindow*>* window_list);
  Browser* CreateRestoredBrowser(Browser::Type type, gfx::Rect bounds,
                                 ui::WindowShowState show_state,
                                 const std::string& app_name);
  content::WebContents* RestoreTab(const sessions::SessionTab& tab,
                                   const int tab_index, Browser* browser,
                                   bool is_selected_tab);
  void NotifySessionServiceOfRestoredTabs(Browser* browser, int initial_count);
  void ShowBrowser(Browser* browser, int selected_tab_index);

  // Reads a single command, returning it. A return value of NULL indicates
  // either there are no commands, or there was an error. Use errored_ to
  // distinguish the two. If NULL is returned, and there is no error, it means
  // the end of file was successfully reached.
  sessions::SessionCommand* ReadCommand();
  const ScopedVector<sessions::SessionCommand>& pending_commands() {
    return pending_commands_;
  }
  // Commands we need to send over to the backend.
  ScopedVector<sessions::SessionCommand> pending_commands_;

  // Maps from session tab id to the range of navigation entries that has
  // been written to disk.
  //
  // This is only used if not all the navigation entries have been
  // written.
  IdToRange tab_to_available_range_;

  // Handle to the target file.
  std::unique_ptr<base::File> current_session_file_;

  // Whether an error condition has been detected
  bool errored_;

  // As we read from the file, data goes here.
  std::string buffer_;

  // Position in buffer_ of the data.
  size_t buffer_position_;

  // Number of available bytes; relative to buffer_position_.
  size_t available_count_;

  // When loading a session, restore the first windows' tabs to this browser
  Browser* browser_;

  Profile* profile_;

  DISALLOW_COPY_AND_ASSIGN(VivaldiSessionService);
};

}  // namespace vivaldi

#endif  // UI_VIVALDI_SESSION_SERVICE_H_
