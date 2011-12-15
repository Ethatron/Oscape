///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __Oscape_Window__
#define __Oscape_Window__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/toolbar.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/filepicker.h>
#include <wx/checklst.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#ifdef __VISUALC__
#include <wx/link_additions.h>
#endif //__VISUALC__
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/scrolwin.h>
#include <wx/statbox.h>
#include <wx/listbook.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_OBLIVON 1000
#define wxID_SKYRIM 1001
#define wxID_DEFV 1002
#define wxID_DEFT 1003
#define wxID_DEFL 1004
#define wxID_LOAD 1005
#define wxID_LEVEL0_INST 1006
#define wxID_LEVEL0UV_INST 1007
#define wxID_LEVEL0_MRES 1008
#define wxID_LEVEL0N_TRES 1009
#define wxID_LEVEL0C_TRES 1010
#define wxID_LEVEL1_INST 1011
#define wxID_LEVEL1UV_INST 1012
#define wxID_LEVEL1_MRES 1013
#define wxID_LEVEL1N_TRES 1014
#define wxID_LEVEL1C_TRES 1015
#define wxID_LEVEL2_INST 1016
#define wxID_LEVEL2UV_INST 1017
#define wxID_LEVEL2_MRES 1018
#define wxID_LEVEL2N_TRES 1019
#define wxID_LEVEL2C_TRES 1020
#define wxID_LEVEL3_INST 1021
#define wxID_LEVEL3UV_INST 1022
#define wxID_LEVEL3_MRES 1023
#define wxID_LEVEL3N_TRES 1024
#define wxID_LEVEL3C_TRES 1025

///////////////////////////////////////////////////////////////////////////////
/// Class wxOscape
///////////////////////////////////////////////////////////////////////////////
class wxOscape : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* OSMenuBar;
		wxMenu* OSGame;
		wxMenu* OSDefaults;
		wxMenu* OSOblivionDefs;
		wxNotebook* OSToolSwitch;
		wxPanel* OSPanelPlugins;
		wxToolBar* OSPluginToolbar;
		wxCheckBox* OSPluginAutosave;
		wxDirPickerCtrl* OSPluginDir;
		wxCheckListBox* OSPluginList;
		wxComboBox* OSWorldspace;
		wxButton* OSWorldspaceFill;
		wxStaticLine* m_staticline7;
		wxStaticText* m_staticText31;
		wxCheckBox* OSCalcHeight;
		wxCheckBox* OSCalcImportance;
		wxCheckBox* OSCalcColor;
		wxFilePickerCtrl* OSFileHeightfieldOut;
		wxButton* OSPluginExtract;
		wxPanel* OSPanelHeightfield;
		wxFilePickerCtrl* OSFileHeightfieldIn1;
		wxStaticText* OSHeightfieldFirst1;
		wxChoice* OSPreviewSelector;
		wxPanel* OSPreview;
		wxCheckBox* OSOrientation;
		wxPropertyGrid* OSHeightfieldInfos;
		wxFilePickerCtrl* OSFilePoints1;
		wxButton* OSPointsClear1;
		wxListCtrl* OSAdditionalPoints;
		wxStaticLine* m_staticline71;
		wxDirPickerCtrl* OSBaseDirOut1;
		wxButton* OSHeightfieldAccept;
		wxPanel* OSPanelGenerator;
		wxFilePickerCtrl* OSFileHeightfieldIn2;
		wxFilePickerCtrl* OSFilePoints2;
		wxButton* OSPointsClear2;
		wxStaticText* OSHeightfieldFirst2;
		wxScrolledWindow* OSSelectGenerator;
		wxStaticText* m_staticText8;
		wxChoice* OSAlgorithm;
		wxStaticText* m_staticText13;
		wxChoice* QSError;
		wxStaticText* m_staticText11;
		wxTextCtrl* OSQThreshold;
		wxStaticText* OSSupersampling;
		wxTextCtrl* OSAThreshold;
		wxStaticText* OSFeatures;
		wxTextCtrl* OSEmphasis;
		wxStaticText* m_staticText12;
		wxTextCtrl* OSTermination;
		wxStaticText* m_staticText15;
		wxComboBox* OSTarget;
		wxStaticLine* m_staticline3;
		wxCheckBox* OSMeshes;
		wxPanel* OSPanelMeshes;
		wxStaticText* m_staticText16;
		wxCheckBox* OSRes1;
		wxCheckBox* OSRes2;
		wxCheckBox* OSRes3;
		wxCheckBox* OSRes4;
		wxStaticText* m_staticText28;
		wxCheckBox* OSRes5;
		wxCheckBox* OSRes6;
		wxCheckBox* OSRes7;
		wxCheckBox* OSRes8;
		wxStaticLine* m_staticline6;
		wxCheckBox* OSMeshBasin;
		wxCheckBox* OSMeshUVs;
		wxCheckBox* OSMeshOpt;
		wxStaticText* m_staticText111;
		wxCheckBox* OSMeshNIF;
		wxCheckBox* OSMeshDX;
		wxCheckBox* OSMeshOBJ;
		wxCheckBox* OSNormals;
		wxPanel* OSPanelNormals;
		wxCheckBox* OSNormalLow;
		wxCheckBox* OSNormalRegular;
		wxCheckBox* OSNormalHigh;
		wxStaticText* m_staticText1111;
		wxCheckBox* OSNormalPPM;
		wxCheckBox* OSNormalDDS;
		wxCheckBox* OSNormalPNG;
		wxCheckBox* OSColors;
		wxPanel* OSPanelColors;
		wxCheckBox* OSColorLow;
		wxCheckBox* OSColorRegular;
		wxCheckBox* OSColorHigh;
		wxCheckBox* OSColorUltra;
		wxStaticText* m_staticText11112;
		wxCheckBox* OSColorPPM;
		wxCheckBox* OSColorDDS;
		wxCheckBox* OSColorPNG;
		wxCheckBox* OSHeightmap;
		wxPanel* OSPanelHeightmap;
		wxCheckBox* OSHeightmapLow;
		wxCheckBox* OSHeightmapRegular;
		wxCheckBox* OSHeightmapHigh;
		wxStaticText* m_staticText11111;
		wxCheckBox* OSHeightmapPPM;
		wxCheckBox* OSHeightmapDDS;
		wxCheckBox* OSHeightmapPNG;
		wxStaticLine* m_staticline711;
		wxCheckBox* OSOverwrite;
		wxDirPickerCtrl* OSBaseDirOut2;
		wxButton* OSHeightfieldGenerate;
		wxPanel* OSPanelInstaller;
		wxDirPickerCtrl* OSBaseDirIn;
		wxStaticText* OSHeightfieldFirst3;
		wxListbook* OSInstallWS;
		wxScrolledWindow* m_scrolledWindow4;
		wxCheckBox* OSInstallLevel0;
		wxCheckBox* OSInstallLevel0UVs;
		wxStaticText* m_staticText40;
		wxChoice* OSInstallLevel0MeshRes;
		wxStaticText* m_staticText401;
		wxChoice* OSInstallLevel0TextResN;
		wxStaticText* m_staticText4014;
		wxChoice* OSInstallLevel0TextResC;
		wxCheckBox* OSInstallLevel1;
		wxCheckBox* OSInstallLevel1UVs;
		wxStaticText* m_staticText402;
		wxChoice* OSInstallLevel1MeshRes;
		wxStaticText* m_staticText4011;
		wxChoice* OSInstallLevel1TextResN;
		wxStaticText* m_staticText40111;
		wxChoice* OSInstallLevel1TextResC;
		wxCheckBox* OSInstallLevel2;
		wxCheckBox* OSInstallLevel2UVs;
		wxStaticText* m_staticText403;
		wxChoice* OSInstallLevel2MeshRes;
		wxStaticText* m_staticText4012;
		wxChoice* OSInstallLevel2TextResN;
		wxStaticText* m_staticText40121;
		wxChoice* OSInstallLevel2TextResC;
		wxCheckBox* OSInstallLevel3;
		wxCheckBox* OSInstallLevel3UVs;
		wxStaticText* m_staticText404;
		wxChoice* OSInstallLevel3MeshRes;
		wxStaticText* m_staticText4013;
		wxChoice* OSInstallLevel3TextResN;
		wxStaticText* m_staticText40131;
		wxChoice* OSInstallLevel3TextResC;
		wxStaticLine* m_staticline7111;
		wxDirPickerCtrl* OSPlugoutDir;
		wxButton* OSHeightfieldInstall;
		wxStatusBar* OSStatusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void ChangeToOblivion( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeToSkyrim( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeDefaults( wxCommandEvent& event ) { event.Skip(); }
		virtual void ResetPluginList( wxCommandEvent& event ) { event.Skip(); }
		virtual void ClearPluginList( wxCommandEvent& event ) { event.Skip(); }
		virtual void LoadPluginList( wxCommandEvent& event ) { event.Skip(); }
		virtual void SavePluginList( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeAutosave( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangePluginDir( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void ChangeActivePlugins( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeWorldspace( wxCommandEvent& event ) { event.Skip(); }
		virtual void WorldspacesFromPlugins( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeHeightfieldOut( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void HeightfieldExtract( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeHeightfieldIn1( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void ChangePreview( wxCommandEvent& event ) { event.Skip(); }
		virtual void PaintH( wxPaintEvent& event ) { event.Skip(); }
		virtual void ChangeOrientation( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangePointsIn1( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void ClearPoints1( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeBaseDirOut1( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void HeightfieldAccept( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeHeightfieldIn2( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void ChangePointsIn2( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void ClearPoints2( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeAlgorithm( wxCommandEvent& event ) { event.Skip(); }
		virtual void CheckFloat( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeTarget( wxCommandEvent& event ) { event.Skip(); }
		virtual void CheckInt( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeMeshes( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeNormals( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeColors( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeHeightmap( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeBaseDirOut2( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void HeightfieldGenerate( wxCommandEvent& event ) { event.Skip(); }
		virtual void ChangeBaseDirIn( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void ChangePlugoutDir( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void HeightfieldInstall( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		wxOscape( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 525,774 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~wxOscape();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class wxProgress
///////////////////////////////////////////////////////////////////////////////
class wxProgress : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel9;
		wxStaticText* OSSubject1;
		wxGauge* OSTask1;
		wxStaticText* OSSubject2;
		wxGauge* OSTask2;
		wxStaticText* m_staticText25;
		wxStaticText* OSRunning;
		wxStaticText* m_staticText27;
		wxStaticText* OSRemaining;
		wxStaticText* m_staticText29;
		wxStaticText* OSTotal;
		wxStaticLine* m_staticline7;
		wxButton* OSPause;
		wxButton* OSAbort;
		
		// Virtual event handlers, overide them in your derived class
		virtual void AbortProgress( wxCloseEvent& event ) { event.Skip(); }
		virtual void IdleProgress( wxIdleEvent& event ) { event.Skip(); }
		virtual void PauseProgress( wxCommandEvent& event ) { event.Skip(); }
		virtual void AbortProgress( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		wxProgress( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Oscape generation in progress ..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 569,265 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~wxProgress();
	
};

#endif //__Oscape_Window__
