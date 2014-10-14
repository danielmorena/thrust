// Copyright (c) 2014 Stanislas Polu.
// See the LICENSE file.

#ifndef EXO_SHELL_BROWSER_SESSION_EXO_SESSION_H_
#define EXO_SHELL_BROWSER_SESSION_EXO_SESSION_H_

#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "net/url_request/url_request_job_factory.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/browser_plugin_guest_manager.h"
#include "content/public/browser/site_instance.h"
#include "content/public/browser/web_contents.h"
#include "brightray/browser/browser_context.h"

#include "src/browser/session/exo_session_cookie_store.h"
#include "src/browser/session/exo_session_visitedlink_store.h"

namespace exo_shell {

class DownloadManagerDelegate;
class ExoShellDevToolsDelegate;
class ResourceContext;
class ExoShellURLRequestContextGetter;
class ExoShellDownloadManagerDelegate;

// ### ExoSession
//
// The ExoSession is BrowserContext passed to an ExoShell to be used with all
// its associated control and frames. A session represents the contextual 
// parameters and data needed to render a web page:
// - Whether or not Local HTML5 Storage is allowed or in-memory
// - An interface to retrieve cookies
// - An interface to retrieve visited links
//
// These interfaces are exposed to Javascript. So that user profiles can be
// handled from there. HTML5 Storage is not exposed to Javascript for the sake
// of simplicity, but we do allow to force it into memory (use of public
// computer)
//
// TODO(spolu): This discrepancy between cookies and local storage may lead to 
// issues as some website may expect to find local storage given the presence of 
// a cookie. Exposing HTML5 Storage to JS is on a future "distant?" roadmap...
// see http://www.w3.org/TR/webstorage/
// - SessionStorage not impacted
// - LocalStorage can be expired / deleted by user so semantics are not too off
class ExoSession : public brightray::BrowserContext,
                   public content::BrowserPluginGuestManager {
public:
  /****************************************************************************/
  /* PUBLIC INTERFACE */
  /****************************************************************************/
  // ### ExoSession
  ExoSession(const bool off_the_record,
             const std::string& path,
             bool dummy_cookie_store = false);
  // ### ~ExoSession
  virtual ~ExoSession();

  /****************************************************************************/
  /* EXOFRAME / DEVTOOLS I/F */
  /****************************************************************************/
  ExoShellDevToolsDelegate* devtools_delegate() {
    return devtools_delegate_;
  }

  // ### GetDevToolsURL
  //
  // Returns the DevTools URL for this session
  GURL GetDevToolsURL();

  ExoSessionCookieStore* GetCookieStore();
  ExoSessionVisitedLinkStore* GetVisitedLinkStore();

  /****************************************************************************/
  /* REQUEST CONTEXT GETTER HELPERS */
  /****************************************************************************/
  net::URLRequestContextGetter* CreateRequestContext(
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors);
  net::URLRequestContextGetter* CreateRequestContextForStoragePartition(
      const base::FilePath& partition_path,
      bool in_memory,
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors);

  /****************************************************************************/
  /* BROWSER CONTEXT IMPLEMENTATION */
  /****************************************************************************/
  virtual base::FilePath GetPath() const OVERRIDE;
  virtual bool IsOffTheRecord() const OVERRIDE;

  virtual content::DownloadManagerDelegate* 
    GetDownloadManagerDelegate() OVERRIDE;

  virtual content::ResourceContext* GetResourceContext() OVERRIDE;
  virtual content::BrowserPluginGuestManager* GetGuestManager() OVERRIDE;

  /****************************************************************************/
  /* BROWSER_PLUGIN_GUEST_MANAGER_IMPLEMENTATION */
  /****************************************************************************/
  virtual content::WebContents* CreateGuest(
      content::SiteInstance* embedder_site_instance,
      int instance_id,
      scoped_ptr<base::DictionaryValue> extra_params) OVERRIDE;
  virtual int GetNextInstanceID() OVERRIDE;
  virtual void MaybeGetGuestByInstanceIDOrKill(
      int guest_instance_id,
      int embedder_render_process_id,
      const GuestByInstanceIDCallback& callback) OVERRIDE;
  virtual bool ForEachGuest(content::WebContents* embedder_web_contents,
                            const GuestCallback& callback) OVERRIDE;

  content::WebContents* GetGuestByInstanceID(int guest_instance_id,
                                             int embedder_render_process_id);

private:
  class ExoResourceContext;

  /****************************************************************************/
  /* PRIVATE INTERFACE                                                        */
  /****************************************************************************/
  virtual void AddGuest(int guest_instance_id,
                        content::WebContents* guest_web_contents);

  void RemoveGuest(int guest_instance_id);

  /****************************************************************************/
  /* MEMBERS                                                                   */
  /****************************************************************************/
  bool                                             off_the_record_;
  bool                                             ignore_certificate_errors_;
  base::FilePath                                   path_;

  scoped_ptr<ExoResourceContext>                   resource_context_;
  scoped_ptr<ExoShellDownloadManagerDelegate>      download_manager_delegate_;
  scoped_refptr<ExoShellURLRequestContextGetter>   url_request_getter_;
  scoped_refptr<ExoSessionCookieStore>             cookie_store_;
  scoped_refptr<ExoSessionVisitedLinkStore>        visitedlink_store_;

  ExoShellDevToolsDelegate*                        devtools_delegate_;

  std::map<int, content::WebContents*>             guest_web_contents_;
  int                                              current_instance_id_;

  friend class ExoSessionCookieStore;
  friend class ExoShellDevToolsDelegate;
  friend class WebViewGuest;
  friend class GuestWebContentsObserver;

  DISALLOW_COPY_AND_ASSIGN(ExoSession);
};

} // namespace exo_shell

#endif // EXO_SHELL_BROWSER_SESSION_EXO_SESSION_H_
