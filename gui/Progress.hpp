/* Version: MPL 1.1/LGPL 3.0
 *
 * "The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is scape.
 * The Modified Code is Oscape.
 *
 * The Original Code is in the public domain.
 * No Rights Reserved.
 *
 * The Initial Developer of the Modified Code is
 * Ethatron <niels@paradice-insight.us>. Portions created by The Initial
 * Developer are Copyright (C) 2011 The Initial Developer.
 * All Rights Reserved.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU Library General Public License Version 3 license (the
 * "LGPL License"), in which case the provisions of LGPL License are
 * applicable instead of those above. If you wish to allow use of your
 * version of this file only under the terms of the LGPL License and not
 * to allow others to use your version of this file under the MPL,
 * indicate your decision by deleting the provisions above and replace
 * them with the notice and other provisions required by the LGPL License.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under either the MPL or the LGPL License."
 */

#ifndef OSCAPE_PROGRESS_HPP
#define OSCAPE_PROGRESS_HPP

#include "../Oscape.h"

// ----------------------------------------------------------------------------
class OscapePrg; extern class OscapePrg *prg;
class OscapePrg : public wxProgress {

private:
  wxEventType evtProgress;
  int idProgress;

  class ProgressEvent: public wxCommandEvent {
  public:
    ProgressEvent(int id, const wxEventType& event_type) : wxCommandEvent(event_type, id) { memset(&state, 0, sizeof(state)); }
    ProgressEvent(const ProgressEvent& event) : wxCommandEvent(event) { memcpy(&state, &event.state, sizeof(state)); }

    wxEvent* Clone() const { return new ProgressEvent(*this); }

  public:
    struct {
      unsigned int mask;

      int task1R; int task1V;
      int task2R; int task2V;
      char subj1S[256];
      char subj2S[256];
    } state;

    void SetTask1Range(int r) { state.mask |= 1; state.task1R = r; }
    void SetTask1Value(int v) { state.mask |= 2; state.task1V = v; }
    void SetTask2Range(int r) { state.mask |= 4; state.task2R = r; }
    void SetTask2Value(int v) { state.mask |= 8; state.task2V = v; }
    void SetSubject1(const char *s) { state.mask |= 16; strcpy(state.subj1S, s); }
    void SetSubject2(const char *s) { state.mask |= 32; strcpy(state.subj2S, s); }
  };

  typedef void (wxEvtHandler::*ProgressEventFunction)(ProgressEvent &);

  /* called from Progress-thread */
  void Progress(ProgressEvent &evt) {
    if (evt.state.mask &  1) OSTask1->SetRange(evt.state.task1R);
    if (evt.state.mask &  2) OSTask1->SetValue(evt.state.task1V);
    if (evt.state.mask &  4) OSTask2->SetRange(evt.state.task2R);
    if (evt.state.mask &  8) OSTask2->SetValue(evt.state.task2V);
    if (evt.state.mask & 16) OSSubject1->SetLabel(evt.state.subj1S);
    if (evt.state.mask & 32) OSSubject2->SetLabel(evt.state.subj2S);
  }

  const char *lastpa;
  const char *lastpb;

public:
  void StartProgress(int rng) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    event.SetTask1Range(rng);

    wxPostEvent(this, event);
  }

  void InitProgress(const char *patterna, size_t val, const char *patternb, double err, int dne, int rng) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    char tmp[256];

    if (patterna) { sprintf(tmp, lastpa = patterna, val); event.SetSubject1(tmp); }
    if (patternb) { sprintf(tmp, lastpb = patternb, err); event.SetSubject2(tmp); }

    event.SetTask1Value(dne);
    event.SetTask2Range(rng);
    event.SetTask2Value(0);

    wxPostEvent(this, event);
  }

  void InitProgress(int rng, Real err) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    char tmp[256];

    sprintf(tmp, lastpb, err); event.SetSubject2(tmp);

    event.SetTask2Range((range1 = rng) * (range2 = 1));
    event.SetTask2Value(0);

    wxPostEvent(this, event);
  }

  void SetProgress(int dne, Real err) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    char tmp[256];

    sprintf(tmp, lastpb, err); event.SetSubject2(tmp);

    event.SetTask2Value(dne);

    wxPostEvent(this, event);
  }

  int range1, value1;
  int range2, value2;

  void InitProgress(int rng) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    event.SetTask2Range((range1 = rng) * (range2 = 1));
    event.SetTask2Value(0);

    wxPostEvent(this, event);
  }

  void SetProgress(int dne) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    event.SetTask2Value((value1 = dne) * (range2) + (value2 = 0));

    wxPostEvent(this, event);
  }

  void SetTopic(const char *topic) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    event.SetSubject2(topic);

    wxPostEvent(this, event);
  }

  void InitProgress(int rng, int srng) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    event.SetTask2Range((range1 = rng) * (range2 = srng));
    event.SetTask2Value(0);

    wxPostEvent(this, event);
  }

  void SetProgress(int dne, int sdne) {
    Wait(); ProgressEvent event(idProgress, evtProgress);

    event.SetTask2Value((value1      ) * (range2) + (value2 = sdne));

    wxPostEvent(this, event);
  }

private:
  bool paused, quit;
  time_t tinit, tlast, tnow;

public:
  bool RequestFeedback(const char *question) {
    wxMessageDialog d(this, question, "Oscape", wxOK | wxCANCEL | wxCENTRE);
    int ret = d.ShowModal();
    if (ret == wxID_CANCEL)
      return false;
    return true;
  }

  virtual void PauseProgress(wxCommandEvent& event) {
    if (!paused) {
      Block();

      paused = true;
      OSPause->SetLabel("Unpause");
    }
    else {
      Unblock();

      paused = false;
      OSPause->SetLabel("Pause");
    }
  }

  void PollProgress() {
    Wait();
  }

  virtual void AbortProgress(wxCloseEvent& event) {
    if (event.CanVeto())
      event.Veto();

    Abort();
  }

  virtual void AbortProgress(wxCommandEvent& event) {
    Abort();
  }

  virtual void IdleProgress(wxIdleEvent& event) {
    tnow = time(NULL);

    long tpass = (long)(tnow - tinit);
    if (tpass != tlast) {
      tlast = tpass;

      float f1 = (float)(OSTask1->GetValue()) / (OSTask1->GetRange());
      float f2 = (float)(OSTask2->GetValue()) / (OSTask1->GetRange() * OSTask2->GetRange());

      int tfinal = (int)floor((float)tpass / (f1 + f2));
      int trem = tfinal - tlast;

      char buf[256];

      sprintf(buf, "%02d:%02ds", tpass / 60, tpass % 60);
      OSRunning->SetLabel(buf);

      if (tfinal != 0x80000000) {
	sprintf(buf, "%02d:%02ds", trem / 60, trem % 60);
	OSRemaining->SetLabel(buf);
	sprintf(buf, "%02d:%02ds", tfinal / 60, tfinal % 60);
	OSTotal->SetLabel(buf);
      }
      else {
	OSRemaining->SetLabel("00:00s");
	OSTotal->SetLabel(buf);
      }
    }

//  Sleep(500);
//  event.RequestMore();
  }

private:
  HANDLE evt, end;
  HANDLE async;

  /* called from Progress-thread */
  void Abort() {
    quit = true;
    SetEvent(evt);
  }

  /* called from Async-thread */
  void Wait() {
    WaitForSingleObject(evt, INFINITE);

    /* signal abortion (inside Async-thread) */
    if (quit)
      throw runtime_error("ExitThread");
  }

  void Block() {
    ResetEvent(evt);
  }

  void Unblock() {
    SetEvent(evt);
  }

public:
  wxEventType evtLeave;
  int idLeave;
  int retCode;

  class LeaveEvent: public wxCommandEvent {
  public:
    LeaveEvent(int id, const wxEventType& event_type) : wxCommandEvent(event_type, id) {}
    LeaveEvent(const LeaveEvent& event) : wxCommandEvent(event) {}

    wxEvent* Clone() const { return new LeaveEvent(*this); }
  };

  typedef void (wxEvtHandler::*LeaveEventFunction)(LeaveEvent &);

  /* called from outside-thread */
  int Enter(LPTHREAD_START_ROUTINE routine) {
    if ((async = CreateThread(NULL, 0, routine, this, 0, NULL)) == INVALID_HANDLE_VALUE)
      return 0;
    SetThreadPriority(async, THREAD_PRIORITY_BELOW_NORMAL);

    return ShowModal();
  }

  /* called from Async-thread */
  void Leave(int rc) {
    retCode = rc;

    LeaveEvent event(idLeave, evtLeave);
    wxPostEvent(this, event);

    /* wait for the progress-dialog to recognize our message */
    WaitForSingleObject(end, INFINITE);
  }

  /* called from Progress-thread */
  void Leave(LeaveEvent &) {
    /* signal that we recognize the message */
    SetEvent(end);

    WaitForSingleObject(async, INFINITE);
    CloseHandle(async);

    EndModal(retCode);
  }

  OscapePrg::OscapePrg(wxWindow *parent) : wxProgress(parent) {
    evtLeave = wxNewEventType();
    idLeave = wxNewId();

    /* Connect to event handler that will make us close */
    Connect(wxID_ANY, evtLeave,
      (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)wxStaticCastEvent(LeaveEventFunction, &OscapePrg::Leave),
      NULL,
      this);

    evtProgress = wxNewEventType();
    idProgress = wxNewId();

    /* Connect to event handler that will make us close */
    Connect(wxID_ANY, evtProgress,
      (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)wxStaticCastEvent(ProgressEventFunction, &OscapePrg::Progress),
      NULL,
      this);

    tinit = time(NULL);
    paused = false;
    quit = false;
    prg = this;

    evt = CreateEvent(
      NULL,		  // default security attributes
      TRUE,		  // manual reset
      TRUE,		  // initially set
      NULL		  // unnamed mutex
      );

    end = CreateEvent(
      NULL,		  // default security attributes
      TRUE,		  // manual reset
      FALSE,		  // initially not set
      NULL		  // unnamed mutex
      );

    SetSize(600, 265);
  }

  OscapePrg::~OscapePrg() {
    CloseHandle(evt);
    CloseHandle(end);
  }
};

#endif
