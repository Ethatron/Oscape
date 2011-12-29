///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Oscape_Window.h"

///////////////////////////////////////////////////////////////////////////

wxOscape::wxOscape( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	OSMenuBar = new wxMenuBar( 0 );
	OSGame = new wxMenu();
	wxMenuItem* OSOblivion;
	OSOblivion = new wxMenuItem( OSGame, wxID_OBLIVON, wxString( wxT("Oblivion") ) , wxEmptyString, wxITEM_RADIO );
	OSGame->Append( OSOblivion );
	OSOblivion->Check( true );
	
	wxMenuItem* OSSkyrim;
	OSSkyrim = new wxMenuItem( OSGame, wxID_SKYRIM, wxString( wxT("Skyrim") ) , wxEmptyString, wxITEM_RADIO );
	OSGame->Append( OSSkyrim );
	
	OSMenuBar->Append( OSGame, wxT("Game") ); 
	
	OSDefaults = new wxMenu();
	OSOblivionDefs = new wxMenu();
	wxMenuItem* OSDefaultVanilla;
	OSDefaultVanilla = new wxMenuItem( OSOblivionDefs, wxID_DEFV, wxString( wxT("Vanilla Oblivion") ) , wxEmptyString, wxITEM_RADIO );
	OSOblivionDefs->Append( OSDefaultVanilla );
	OSDefaultVanilla->Check( true );
	
	wxMenuItem* OSDefaultTWMP;
	OSDefaultTWMP = new wxMenuItem( OSOblivionDefs, wxID_DEFT, wxString( wxT("Vanilla TWMP") ) , wxEmptyString, wxITEM_RADIO );
	OSOblivionDefs->Append( OSDefaultTWMP );
	
	wxMenuItem* OSDefaultLLOD;
	OSDefaultLLOD = new wxMenuItem( OSOblivionDefs, wxID_DEFL, wxString( wxT("LLOD TWMP") ) , wxEmptyString, wxITEM_RADIO );
	OSOblivionDefs->Append( OSDefaultLLOD );
	
	OSDefaults->Append( -1, wxT("Oblivion"), OSOblivionDefs );
	
	OSMenuBar->Append( OSDefaults, wxT("Defaults") ); 
	
	OSSettings = new wxMenu();
	OSTSampling = new wxMenu();
	wxMenuItem* OSTS1x1;
	OSTS1x1 = new wxMenuItem( OSTSampling, wxID_TS1, wxString( wxT("1x1") ) , wxEmptyString, wxITEM_RADIO );
	OSTSampling->Append( OSTS1x1 );
	OSTS1x1->Check( true );
	
	wxMenuItem* OSTS2x2;
	OSTS2x2 = new wxMenuItem( OSTSampling, wxID_TS2, wxString( wxT("2x2") ) , wxEmptyString, wxITEM_RADIO );
	OSTSampling->Append( OSTS2x2 );
	
	wxMenuItem* OSTS4x4;
	OSTS4x4 = new wxMenuItem( OSTSampling, wxID_TS4, wxString( wxT("4x4") ) , wxEmptyString, wxITEM_RADIO );
	OSTSampling->Append( OSTS4x4 );
	
	OSSettings->Append( -1, wxT("Texture super-sampling"), OSTSampling );
	
	OSSSampling = new wxMenu();
	wxMenuItem* OSS1x;
	OSS1x = new wxMenuItem( OSSSampling, wxID_SS1, wxString( wxT("1x") ) , wxEmptyString, wxITEM_RADIO );
	OSSSampling->Append( OSS1x );
	OSS1x->Check( true );
	
	wxMenuItem* OSS2x;
	OSS2x = new wxMenuItem( OSSSampling, wxID_SS2, wxString( wxT("2x") ) , wxEmptyString, wxITEM_RADIO );
	OSSSampling->Append( OSS2x );
	
	wxMenuItem* OSS4x;
	OSS4x = new wxMenuItem( OSSSampling, wxID_SS4, wxString( wxT("4x") ) , wxEmptyString, wxITEM_RADIO );
	OSSSampling->Append( OSS4x );
	
	OSSettings->Append( -1, wxT("Surface up-sampling"), OSSSampling );
	
	OSMenuBar->Append( OSSettings, wxT("Settings") ); 
	
	this->SetMenuBar( OSMenuBar );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	OSToolSwitch = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	OSPanelPlugins = new wxPanel( OSToolSwitch, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxHORIZONTAL );
	
	OSPluginToolbar = new wxToolBar( OSPanelPlugins, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
	OSPluginToolbar->AddTool( wxID_RESET, wxT("Reset"), wxBitmap( wxT("#112"), wxBITMAP_TYPE_RESOURCE ), wxNullBitmap, wxITEM_NORMAL, wxT("Selects the plugins from O's load-order"), wxEmptyString, NULL ); 
	OSPluginToolbar->AddTool( wxID_CLEAR, wxT("tool"), wxBitmap( wxT("#110"), wxBITMAP_TYPE_RESOURCE ), wxNullBitmap, wxITEM_NORMAL, wxT("Clears the selection"), wxEmptyString, NULL ); 
	OSPluginToolbar->AddTool( wxID_LOAD, wxT("Load"), wxBitmap( wxT("#103"), wxBITMAP_TYPE_RESOURCE ), wxNullBitmap, wxITEM_NORMAL, wxT("Loads your last saved state"), wxEmptyString, NULL ); 
	OSPluginToolbar->AddTool( wxID_SAVE, wxT("Save"), wxBitmap( wxT("#104"), wxBITMAP_TYPE_RESOURCE ), wxNullBitmap, wxITEM_NORMAL, wxT("Save your current state"), wxEmptyString, NULL ); 
	OSPluginToolbar->Realize();
	
	bSizer39->Add( OSPluginToolbar, 0, wxEXPAND, 5 );
	
	OSPluginAutosave = new wxCheckBox( OSPanelPlugins, wxID_ANY, wxT("Autosave selection on success"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer39->Add( OSPluginAutosave, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	bSizer36->Add( bSizer39, 0, wxEXPAND, 5 );
	
	OSPluginDir = new wxDirPickerCtrl( OSPanelPlugins, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL );
	OSPluginDir->SetHelpText( wxT("The directory with the plugin-files. Most often the \"Data/\"-folder.") );
	
	bSizer36->Add( OSPluginDir, 0, wxALL|wxEXPAND, 5 );
	
	wxString OSPluginListChoices[] = { wxT("a.esp"), wxT("b.esp") };
	int OSPluginListNChoices = sizeof( OSPluginListChoices ) / sizeof( wxString );
	OSPluginList = new wxCheckListBox( OSPanelPlugins, wxID_ANY, wxDefaultPosition, wxDefaultSize, OSPluginListNChoices, OSPluginListChoices, 0 );
	OSPluginList->SetToolTip( wxT("The plug-ins which should be used to extract the height-field") );
	
	bSizer36->Add( OSPluginList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	OSWorldspace = new wxComboBox( OSPanelPlugins, wxID_ANY, wxT("Tamriel"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	OSWorldspace->Append( wxT("Tamriel") );
	OSWorldspace->Append( wxT("SEWorld") );
	OSWorldspace->SetToolTip( wxT("The worldspace of which you want to extract data") );
	
	bSizer14->Add( OSWorldspace, 1, wxALL, 5 );
	
	OSWorldspaceFill = new wxButton( OSPanelPlugins, wxID_ANY, wxT("Fill"), wxDefaultPosition, wxDefaultSize, 0 );
	OSWorldspaceFill->Enable( false );
	OSWorldspaceFill->SetToolTip( wxT("Fill the worldspace-selector with the available ones from the plugin-list") );
	
	bSizer14->Add( OSWorldspaceFill, 0, wxALL, 5 );
	
	bSizer36->Add( bSizer14, 0, wxEXPAND, 5 );
	
	m_staticline7 = new wxStaticLine( OSPanelPlugins, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer36->Add( m_staticline7, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText31 = new wxStaticText( OSPanelPlugins, wxID_ANY, wxT("Outputs:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	bSizer32->Add( m_staticText31, 1, wxALL, 5 );
	
	OSCalcHeight = new wxCheckBox( OSPanelPlugins, wxID_ANY, wxT("heightfield"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSCalcHeight->SetValue(true); 
	OSCalcHeight->Enable( false );
	
	bSizer32->Add( OSCalcHeight, 0, wxALL, 5 );
	
	OSCalcImportance = new wxCheckBox( OSPanelPlugins, wxID_ANY, wxT("feature-map"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	bSizer32->Add( OSCalcImportance, 0, wxALL, 5 );
	
	OSCalcColor = new wxCheckBox( OSPanelPlugins, wxID_ANY, wxT("surface-map"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	bSizer32->Add( OSCalcColor, 0, wxALL, 5 );
	
	bSizer36->Add( bSizer32, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer( wxHORIZONTAL );
	
	OSFileHeightfieldOut = new wxFilePickerCtrl( OSPanelPlugins, wxID_ANY, wxT("./Tamriel.raw"), wxT("Select a file"), wxT("*.raw"), wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL );
	bSizer42->Add( OSFileHeightfieldOut, 1, wxALL, 5 );
	
	OSPluginExtract = new wxButton( OSPanelPlugins, wxID_ANY, wxT("Extract"), wxDefaultPosition, wxDefaultSize, 0 );
	OSPluginExtract->Enable( false );
	OSPluginExtract->SetToolTip( wxT("Start extract the desired data to the given file (and related files)") );
	
	bSizer42->Add( OSPluginExtract, 0, wxALL, 5 );
	
	bSizer36->Add( bSizer42, 0, wxEXPAND, 5 );
	
	OSPanelPlugins->SetSizer( bSizer36 );
	OSPanelPlugins->Layout();
	bSizer36->Fit( OSPanelPlugins );
	OSToolSwitch->AddPage( OSPanelPlugins, wxT("Active Plugins"), true );
	OSPanelHeightfield = new wxPanel( OSToolSwitch, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxVERTICAL );
	
	OSFileHeightfieldIn1 = new wxFilePickerCtrl( OSPanelHeightfield, wxID_ANY, wxT("./Tamriel.raw"), wxT("Select a file"), wxT("*.raw"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	bSizer37->Add( OSFileHeightfieldIn1, 0, wxALL|wxEXPAND, 5 );
	
	OSHeightfieldFirst1 = new wxStaticText( OSPanelHeightfield, wxID_ANY, wxT("Select a heightfield first"), wxDefaultPosition, wxDefaultSize, 0 );
	OSHeightfieldFirst1->Wrap( -1 );
	bSizer37->Add( OSHeightfieldFirst1, 0, wxALIGN_CENTER|wxALL, 25 );
	
	wxString OSPreviewSelectorChoices[] = { wxT("Normals"), wxT("Heightfield"), wxT("Importance"), wxT("Color") };
	int OSPreviewSelectorNChoices = sizeof( OSPreviewSelectorChoices ) / sizeof( wxString );
	OSPreviewSelector = new wxChoice( OSPanelHeightfield, wxID_ANY, wxDefaultPosition, wxDefaultSize, OSPreviewSelectorNChoices, OSPreviewSelectorChoices, 0 );
	OSPreviewSelector->SetSelection( 1 );
	OSPreviewSelector->Hide();
	
	bSizer37->Add( OSPreviewSelector, 0, wxALL|wxEXPAND, 5 );
	
	OSPreview = new wxPanel( OSPanelHeightfield, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
	OSPreview->SetToolTip( wxT("ough preview of the height-field") );
	
	bSizer37->Add( OSPreview, 1, wxEXPAND | wxALL, 5 );
	
	OSOrientation = new wxCheckBox( OSPanelHeightfield, wxID_ANY, wxT("horizontal orientation"), wxDefaultPosition, wxDefaultSize, 0 );
	OSOrientation->SetValue(true); 
	OSOrientation->SetToolTip( wxT("Give a hint to the program which is the orientation of the data.") );
	
	bSizer37->Add( OSOrientation, 0, wxALL, 5 );
	
	OSHeightfieldInfos = new wxPropertyGrid(OSPanelHeightfield, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_BOLD_MODIFIED|wxPG_DEFAULT_STYLE);
	bSizer37->Add( OSHeightfieldInfos, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );
	
	OSFilePoints1 = new wxFilePickerCtrl( OSPanelHeightfield, wxID_ANY, wxT("./Tamriel.pts"), wxT("Select a file"), wxT("*.pts"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	bSizer19->Add( OSFilePoints1, 1, wxALL|wxEXPAND, 5 );
	
	OSPointsClear1 = new wxButton( OSPanelHeightfield, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	OSPointsClear1->Enable( false );
	OSPointsClear1->SetToolTip( wxT("Clear the points-file") );
	
	bSizer19->Add( OSPointsClear1, 0, wxALL, 5 );
	
	bSizer37->Add( bSizer19, 0, wxEXPAND, 5 );
	
	OSAdditionalPoints = new wxListCtrl( OSPanelHeightfield, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_LIST );
	bSizer37->Add( OSAdditionalPoints, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline71 = new wxStaticLine( OSPanelHeightfield, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer37->Add( m_staticline71, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer421;
	bSizer421 = new wxBoxSizer( wxHORIZONTAL );
	
	OSBaseDirOut1 = new wxDirPickerCtrl( OSPanelHeightfield, wxID_ANY, wxT("./generated"), wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL );
	OSBaseDirOut1->SetToolTip( wxT("The directory into which to put the generated files") );
	
	bSizer421->Add( OSBaseDirOut1, 1, wxALL, 5 );
	
	OSHeightfieldAccept = new wxButton( OSPanelHeightfield, wxID_ANY, wxT("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	OSHeightfieldAccept->Enable( false );
	OSHeightfieldAccept->SetToolTip( wxT("Accept the current values and go to the generator-page") );
	
	bSizer421->Add( OSHeightfieldAccept, 0, wxALL, 5 );
	
	bSizer37->Add( bSizer421, 0, wxEXPAND, 5 );
	
	OSPanelHeightfield->SetSizer( bSizer37 );
	OSPanelHeightfield->Layout();
	bSizer37->Fit( OSPanelHeightfield );
	OSToolSwitch->AddPage( OSPanelHeightfield, wxT("Preview"), false );
	OSPanelGenerator = new wxPanel( OSToolSwitch, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	OSFileHeightfieldIn2 = new wxFilePickerCtrl( OSPanelGenerator, wxID_ANY, wxT("./Tamriel.raw"), wxT("Select a file"), wxT("*.raw"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	bSizer15->Add( OSFileHeightfieldIn2, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxHORIZONTAL );
	
	OSFilePoints2 = new wxFilePickerCtrl( OSPanelGenerator, wxID_ANY, wxT("./Tamriel.pts"), wxT("Select a file"), wxT("*.pts"), wxDefaultPosition, wxDefaultSize, wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	bSizer191->Add( OSFilePoints2, 1, wxALL|wxEXPAND, 5 );
	
	OSPointsClear2 = new wxButton( OSPanelGenerator, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	OSPointsClear2->Enable( false );
	OSPointsClear2->SetToolTip( wxT("Clear the points-file") );
	
	bSizer191->Add( OSPointsClear2, 0, wxALL, 5 );
	
	bSizer15->Add( bSizer191, 0, wxEXPAND, 5 );
	
	OSHeightfieldFirst2 = new wxStaticText( OSPanelGenerator, wxID_ANY, wxT("Select a heightfield first"), wxDefaultPosition, wxDefaultSize, 0 );
	OSHeightfieldFirst2->Wrap( -1 );
	bSizer15->Add( OSHeightfieldFirst2, 1, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 25 );
	
	OSSelectGenerator = new wxScrolledWindow( OSPanelGenerator, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	OSSelectGenerator->SetScrollRate( 5, 5 );
	OSSelectGenerator->Hide();
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	wxGridSizer* gSizer10;
	gSizer10 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_staticText8 = new wxStaticText( OSSelectGenerator, wxID_ANY, wxT("Algorithm:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	gSizer10->Add( m_staticText8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString OSAlgorithmChoices[] = { wxT("Data-dependent"), wxT("Delaunay") };
	int OSAlgorithmNChoices = sizeof( OSAlgorithmChoices ) / sizeof( wxString );
	OSAlgorithm = new wxChoice( OSSelectGenerator, wxID_ANY, wxDefaultPosition, wxDefaultSize, OSAlgorithmNChoices, OSAlgorithmChoices, 0 );
	OSAlgorithm->SetSelection( 0 );
	OSAlgorithm->SetToolTip( wxT("Data-dependent is best") );
	
	gSizer10->Add( OSAlgorithm, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText13 = new wxStaticText( OSSelectGenerator, wxID_ANY, wxT("Error selection:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	gSizer10->Add( m_staticText13, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString QSErrorChoices[] = { wxT("Sum of max. error"), wxT("Max of max. error"), wxT("Sum of error^2"), wxT("Angle of normals") };
	int QSErrorNChoices = sizeof( QSErrorChoices ) / sizeof( wxString );
	QSError = new wxChoice( OSSelectGenerator, wxID_ANY, wxDefaultPosition, wxDefaultSize, QSErrorNChoices, QSErrorChoices, 0 );
	QSError->SetSelection( 2 );
	QSError->SetToolTip( wxT("Sum of error^2 is best") );
	
	gSizer10->Add( QSError, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText11 = new wxStaticText( OSSelectGenerator, wxID_ANY, wxT("Quality threshold from 1.0 to 0.0:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	gSizer10->Add( m_staticText11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OSQThreshold = new wxTextCtrl( OSSelectGenerator, wxID_ANY, wxT("0.95"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	OSQThreshold->SetToolTip( wxT("0.95 is best, 0.0 is completely data-dependent, below 0.25 is not recommended") );
	
	gSizer10->Add( OSQThreshold, 0, wxALL|wxEXPAND, 5 );
	
	OSSupersampling = new wxStaticText( OSSelectGenerator, wxID_ANY, wxT("Area threshold from 0.0 to inf.:"), wxDefaultPosition, wxDefaultSize, 0 );
	OSSupersampling->Wrap( -1 );
	OSSupersampling->SetToolTip( wxT("Maximum area allowed without super-sampling") );
	
	gSizer10->Add( OSSupersampling, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OSAThreshold = new wxTextCtrl( OSSelectGenerator, wxID_ANY, wxT("1e30"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	OSAThreshold->SetToolTip( wxT("1e30 is default, when to supersample") );
	
	gSizer10->Add( OSAThreshold, 0, wxALL|wxEXPAND, 5 );
	
	OSFeatures = new wxStaticText( OSSelectGenerator, wxID_ANY, wxT("Emphasis of features:"), wxDefaultPosition, wxDefaultSize, 0 );
	OSFeatures->Wrap( -1 );
	OSFeatures->SetToolTip( wxT("Maximum area allowed without super-sampling") );
	
	gSizer10->Add( OSFeatures, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OSEmphasis = new wxTextCtrl( OSSelectGenerator, wxID_ANY, wxT("0.0"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	OSEmphasis->SetToolTip( wxT("0.0 is default, defines the emphasis of the feature-map, 0.001 is a reasonable value") );
	
	gSizer10->Add( OSEmphasis, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText12 = new wxStaticText( OSSelectGenerator, wxID_ANY, wxT("Termination at error:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	gSizer10->Add( m_staticText12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OSTermination = new wxTextCtrl( OSSelectGenerator, wxID_ANY, wxT("0.00"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	OSTermination->SetToolTip( wxT("Causes premature termination in case the error-threshold is reached") );
	
	gSizer10->Add( OSTermination, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText15 = new wxStaticText( OSSelectGenerator, wxID_ANY, wxT("Target of max. resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	gSizer10->Add( m_staticText15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OSTarget = new wxComboBox( OSSelectGenerator, wxID_ANY, wxT("786432"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	OSTarget->Append( wxT("786432") );
	OSTarget->Append( wxT("393216") );
	OSTarget->Append( wxT("196608") );
	OSTarget->Append( wxT("98304") );
	OSTarget->Append( wxT("49152") );
	OSTarget->Append( wxT("24576") );
	OSTarget->Append( wxT("12288") );
	OSTarget->Append( wxT("131072") );
	OSTarget->Append( wxT("65536") );
	OSTarget->Append( wxT("32768") );
	OSTarget->Append( wxT("16384") );
	OSTarget->Append( wxT("8192") );
	OSTarget->Append( wxT("4096") );
	OSTarget->Append( wxT("2048") );
	OSTarget->SetToolTip( wxT("Number of points for the entire data-set") );
	
	gSizer10->Add( OSTarget, 0, wxALL|wxEXPAND, 5 );
	
	bSizer38->Add( gSizer10, 0, wxEXPAND, 5 );
	
	m_staticline3 = new wxStaticLine( OSSelectGenerator, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer38->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	OSMeshes = new wxCheckBox( OSSelectGenerator, wxID_ANY, wxT("Meshes"), wxDefaultPosition, wxDefaultSize, 0 );
	OSMeshes->SetValue(true); 
	OSMeshes->SetToolTip( wxT("Generate meshes") );
	
	bSizer38->Add( OSMeshes, 0, wxALL, 5 );
	
	OSPanelMeshes = new wxPanel( OSSelectGenerator, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSTATIC_BORDER|wxTAB_TRAVERSAL );
	OSPanelMeshes->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT ) );
	
	wxBoxSizer* bSizer141;
	bSizer141 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer70;
	bSizer70 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText16 = new wxStaticText( OSPanelMeshes, wxID_ANY, wxT("LODs:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	bSizer70->Add( m_staticText16, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OSMlod1 = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("1/1"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMlod1->SetValue(true); 
	OSMlod1->Enable( false );
	
	bSizer70->Add( OSMlod1, 0, wxALL, 5 );
	
	OSMlod2 = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("1/2"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMlod2->SetValue(true); 
	bSizer70->Add( OSMlod2, 0, wxALL, 5 );
	
	OSMlod3 = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("1/4"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMlod3->SetValue(true); 
	bSizer70->Add( OSMlod3, 0, wxALL, 5 );
	
	OSMlod4 = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("1/8"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMlod4->SetValue(true); 
	bSizer70->Add( OSMlod4, 0, wxALL, 5 );
	
	bSizer24->Add( bSizer70, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer701;
	bSizer701 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText28 = new wxStaticText( OSPanelMeshes, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28->Wrap( -1 );
	bSizer701->Add( m_staticText28, 1, wxALL, 5 );
	
	OSMlod5 = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("1/16"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMlod5->SetValue(true); 
	bSizer701->Add( OSMlod5, 0, wxALL, 5 );
	
	OSMlod6 = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("1/32"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMlod6->SetValue(true); 
	bSizer701->Add( OSMlod6, 0, wxALL, 5 );
	
	OSMlod7 = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("1/64"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMlod7->SetValue(true); 
	bSizer701->Add( OSMlod7, 0, wxALL, 5 );
	
	OSMlod8 = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("1/128"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMlod8->SetValue(true); 
	bSizer701->Add( OSMlod8, 0, wxALL, 5 );
	
	bSizer24->Add( bSizer701, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7011;
	bSizer7011 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText281 = new wxStaticText( OSPanelMeshes, wxID_ANY, wxT("LOD Reduction:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText281->Wrap( -1 );
	m_staticText281->Hide();
	
	bSizer7011->Add( m_staticText281, 1, wxALL, 5 );
	
	OSMlodHalf = new wxRadioButton( OSPanelMeshes, wxID_ANY, wxT("0.5"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	OSMlodHalf->SetValue( true ); 
	OSMlodHalf->Hide();
	OSMlodHalf->SetHelpText( wxT("Adjust target each LOD by 0.5 (proportional to extent reduction)") );
	
	bSizer7011->Add( OSMlodHalf, 0, wxALL, 5 );
	
	OSMlodQuat = new wxRadioButton( OSPanelMeshes, wxID_ANY, wxT("0.5^1.5"), wxDefaultPosition, wxDefaultSize, 0 );
	OSMlodQuat->Hide();
	OSMlodQuat->SetHelpText( wxT("Adjust target each LOD by 0.5^1.5") );
	
	bSizer7011->Add( OSMlodQuat, 0, wxALL, 5 );
	
	OSMlodCube = new wxRadioButton( OSPanelMeshes, wxID_ANY, wxT("0.5^2"), wxDefaultPosition, wxDefaultSize, 0 );
	OSMlodCube->Hide();
	OSMlodCube->SetHelpText( wxT("Adjust target each LOD by 0.5^2.0 (proportional to area reduction)") );
	
	bSizer7011->Add( OSMlodCube, 0, wxALL, 5 );
	
	bSizer24->Add( bSizer7011, 1, wxEXPAND, 5 );
	
	bSizer141->Add( bSizer24, 0, wxEXPAND, 5 );
	
	m_staticline6 = new wxStaticLine( OSPanelMeshes, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer141->Add( m_staticline6, 0, wxEXPAND | wxALL, 5 );
	
	OSMeshBasin = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("with ocean-basin"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMeshBasin->SetToolTip( wxT("The ocean won't be cut of at 512 units") );
	
	bSizer141->Add( OSMeshBasin, 0, wxALL|wxRIGHT|wxEXPAND, 5 );
	
	OSMeshUVs = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("with texture-coordinates (UVs)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxCHK_3STATE|wxCHK_ALLOW_3RD_STATE_FOR_USER );
	OSMeshUVs->SetToolTip( wxT("The meshes will contain UV-coordinates and don't require OBGE") );
	
	bSizer141->Add( OSMeshUVs, 0, wxALL|wxEXPAND, 5 );
	
	OSMeshOpt = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("optimize for overdraw"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMeshOpt->SetValue(true); 
	OSMeshOpt->SetToolTip( wxT("Optimize the meshes to render faster") );
	
	bSizer141->Add( OSMeshOpt, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText111 = new wxStaticText( OSPanelMeshes, wxID_ANY, wxT("Formats:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	bSizer17->Add( m_staticText111, 1, wxALL|wxEXPAND, 5 );
	
	OSMeshNIF = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("NIF"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMeshNIF->SetValue(true); 
	OSMeshNIF->SetToolTip( wxT("generate meshes in Oblivion's standard format") );
	
	bSizer17->Add( OSMeshNIF, 0, wxALL, 5 );
	
	OSMeshDX = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("DirectX"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMeshDX->SetValue(true); 
	OSMeshDX->SetToolTip( wxT("generate meshes in Direct-X format as well, requires OBGE") );
	
	bSizer17->Add( OSMeshDX, 0, wxALL, 5 );
	
	OSMeshOBJ = new wxCheckBox( OSPanelMeshes, wxID_ANY, wxT("Wavefront"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSMeshOBJ->SetToolTip( wxT("generate meshes in Wavefront-format if you want to import it into a 3D-package") );
	
	bSizer17->Add( OSMeshOBJ, 0, wxALL, 5 );
	
	bSizer141->Add( bSizer17, 1, wxEXPAND, 5 );
	
	OSPanelMeshes->SetSizer( bSizer141 );
	OSPanelMeshes->Layout();
	bSizer141->Fit( OSPanelMeshes );
	bSizer38->Add( OSPanelMeshes, 0, wxEXPAND | wxALL, 5 );
	
	OSNormals = new wxCheckBox( OSSelectGenerator, wxID_ANY, wxT("Normals"), wxDefaultPosition, wxDefaultSize, 0 );
	OSNormals->SetValue(true); 
	OSNormals->SetToolTip( wxT("Generate normal-maps") );
	
	bSizer38->Add( OSNormals, 0, wxALL, 5 );
	
	OSPanelNormals = new wxPanel( OSSelectGenerator, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSTATIC_BORDER|wxTAB_TRAVERSAL );
	OSPanelNormals->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT ) );
	
	wxBoxSizer* bSizer1411;
	bSizer1411 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer241;
	bSizer241 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer702;
	bSizer702 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText161 = new wxStaticText( OSPanelNormals, wxID_ANY, wxT("LODs:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText161->Wrap( -1 );
	bSizer702->Add( m_staticText161, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OSNlod1 = new wxCheckBox( OSPanelNormals, wxID_ANY, wxT("1/1"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSNlod1->SetValue(true); 
	OSNlod1->Enable( false );
	
	bSizer702->Add( OSNlod1, 0, wxALL, 5 );
	
	OSNlod2 = new wxCheckBox( OSPanelNormals, wxID_ANY, wxT("1/2"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSNlod2->SetValue(true); 
	bSizer702->Add( OSNlod2, 0, wxALL, 5 );
	
	OSNlod3 = new wxCheckBox( OSPanelNormals, wxID_ANY, wxT("1/4"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSNlod3->SetValue(true); 
	bSizer702->Add( OSNlod3, 0, wxALL, 5 );
	
	OSNlod4 = new wxCheckBox( OSPanelNormals, wxID_ANY, wxT("1/8"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSNlod4->SetValue(true); 
	bSizer702->Add( OSNlod4, 0, wxALL, 5 );
	
	bSizer241->Add( bSizer702, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer1411->Add( bSizer241, 1, wxEXPAND, 5 );
	
	m_staticline12 = new wxStaticLine( OSPanelNormals, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1411->Add( m_staticline12, 0, wxEXPAND | wxALL, 5 );
	
	OSNormalLow = new wxCheckBox( OSPanelNormals, wxID_ANY, wxT("low-resolution textures (512x512)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSNormalLow->SetValue(true); 
	bSizer1411->Add( OSNormalLow, 0, wxALL|wxRIGHT|wxEXPAND, 5 );
	
	OSNormalRegular = new wxCheckBox( OSPanelNormals, wxID_ANY, wxT("standard-resolution textures (1024x1024)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSNormalRegular->SetValue(true); 
	bSizer1411->Add( OSNormalRegular, 0, wxALL|wxEXPAND, 5 );
	
	OSNormalHigh = new wxCheckBox( OSPanelNormals, wxID_ANY, wxT("high-resolution textures (2048x2048)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSNormalHigh->SetValue(true); 
	bSizer1411->Add( OSNormalHigh, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer171;
	bSizer171 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1111 = new wxStaticText( OSPanelNormals, wxID_ANY, wxT("Formats:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1111->Wrap( -1 );
	bSizer171->Add( m_staticText1111, 1, wxALL|wxEXPAND, 5 );
	
	OSNormalPPM = new wxCheckBox( OSPanelNormals, wxID_ANY, wxT("PPM"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	bSizer171->Add( OSNormalPPM, 0, wxALL, 5 );
	
	OSNormalDDS = new wxCheckBox( OSPanelNormals, wxID_ANY, wxT("DirectX"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSNormalDDS->SetValue(true); 
	bSizer171->Add( OSNormalDDS, 0, wxALL, 5 );
	
	OSNormalPNG = new wxCheckBox( OSPanelNormals, wxID_ANY, wxT("PNG"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSNormalPNG->SetValue(true); 
	bSizer171->Add( OSNormalPNG, 0, wxALL, 5 );
	
	bSizer1411->Add( bSizer171, 1, wxEXPAND, 5 );
	
	OSPanelNormals->SetSizer( bSizer1411 );
	OSPanelNormals->Layout();
	bSizer1411->Fit( OSPanelNormals );
	bSizer38->Add( OSPanelNormals, 0, wxALL|wxEXPAND, 5 );
	
	OSColors = new wxCheckBox( OSSelectGenerator, wxID_ANY, wxT("Colors"), wxDefaultPosition, wxDefaultSize, 0 );
	OSColors->SetToolTip( wxT("Generate color-maps") );
	
	bSizer38->Add( OSColors, 0, wxALL, 5 );
	
	OSPanelColors = new wxPanel( OSSelectGenerator, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSTATIC_BORDER|wxTAB_TRAVERSAL );
	OSPanelColors->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT ) );
	OSPanelColors->Hide();
	
	wxBoxSizer* bSizer14112;
	bSizer14112 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2411;
	bSizer2411 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer7021;
	bSizer7021 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1611 = new wxStaticText( OSPanelColors, wxID_ANY, wxT("LODs:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1611->Wrap( -1 );
	bSizer7021->Add( m_staticText1611, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OSClod1 = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("1/1"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSClod1->SetValue(true); 
	OSClod1->Enable( false );
	
	bSizer7021->Add( OSClod1, 0, wxALL, 5 );
	
	OSClod2 = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("1/2"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSClod2->SetValue(true); 
	bSizer7021->Add( OSClod2, 0, wxALL, 5 );
	
	OSClod3 = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("1/4"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSClod3->SetValue(true); 
	bSizer7021->Add( OSClod3, 0, wxALL, 5 );
	
	OSClod4 = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("1/8"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSClod4->SetValue(true); 
	bSizer7021->Add( OSClod4, 0, wxALL, 5 );
	
	bSizer2411->Add( bSizer7021, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer14112->Add( bSizer2411, 1, wxEXPAND, 5 );
	
	m_staticline13 = new wxStaticLine( OSPanelColors, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer14112->Add( m_staticline13, 0, wxEXPAND | wxALL, 5 );
	
	OSColorLow = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("low-resolution textures (512x512)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSColorLow->SetValue(true); 
	bSizer14112->Add( OSColorLow, 0, wxALL|wxRIGHT|wxEXPAND, 5 );
	
	OSColorRegular = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("standard-resolution textures (1024x1024)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSColorRegular->SetValue(true); 
	bSizer14112->Add( OSColorRegular, 0, wxALL|wxEXPAND, 5 );
	
	OSColorHigh = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("high-resolution textures (2048x2048)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSColorHigh->Enable( false );
	
	bSizer14112->Add( OSColorHigh, 0, wxALL|wxEXPAND, 5 );
	
	OSColorUltra = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("very high-resolution textures (4096x4096)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSColorUltra->Enable( false );
	
	bSizer14112->Add( OSColorUltra, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1712;
	bSizer1712 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText11112 = new wxStaticText( OSPanelColors, wxID_ANY, wxT("Formats:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11112->Wrap( -1 );
	bSizer1712->Add( m_staticText11112, 1, wxALL|wxEXPAND, 5 );
	
	OSColorPPM = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("PPM"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	bSizer1712->Add( OSColorPPM, 0, wxALL, 5 );
	
	OSColorDDS = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("DirectX"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSColorDDS->SetValue(true); 
	bSizer1712->Add( OSColorDDS, 0, wxALL, 5 );
	
	OSColorPNG = new wxCheckBox( OSPanelColors, wxID_ANY, wxT("PNG"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	OSColorPNG->SetValue(true); 
	bSizer1712->Add( OSColorPNG, 0, wxALL, 5 );
	
	bSizer14112->Add( bSizer1712, 1, wxEXPAND, 5 );
	
	OSPanelColors->SetSizer( bSizer14112 );
	OSPanelColors->Layout();
	bSizer14112->Fit( OSPanelColors );
	bSizer38->Add( OSPanelColors, 0, wxEXPAND | wxALL, 5 );
	
	OSHeightmap = new wxCheckBox( OSSelectGenerator, wxID_ANY, wxT("Heightmaps"), wxDefaultPosition, wxDefaultSize, 0 );
	OSHeightmap->Enable( false );
	
	bSizer38->Add( OSHeightmap, 0, wxALL, 5 );
	
	OSPanelHeightmap = new wxPanel( OSSelectGenerator, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSTATIC_BORDER|wxTAB_TRAVERSAL );
	OSPanelHeightmap->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT ) );
	OSPanelHeightmap->Hide();
	
	wxBoxSizer* bSizer14111;
	bSizer14111 = new wxBoxSizer( wxVERTICAL );
	
	OSHeightmapLow = new wxCheckBox( OSPanelHeightmap, wxID_ANY, wxT("low-resolution textures (512x512)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	bSizer14111->Add( OSHeightmapLow, 0, wxALL|wxRIGHT|wxEXPAND, 5 );
	
	OSHeightmapRegular = new wxCheckBox( OSPanelHeightmap, wxID_ANY, wxT("standard-resolution textures (1024x1024)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	bSizer14111->Add( OSHeightmapRegular, 0, wxALL|wxEXPAND, 5 );
	
	OSHeightmapHigh = new wxCheckBox( OSPanelHeightmap, wxID_ANY, wxT("high-resolution textures (2048x20484)"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	bSizer14111->Add( OSHeightmapHigh, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1711;
	bSizer1711 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText11111 = new wxStaticText( OSPanelHeightmap, wxID_ANY, wxT("Formats:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11111->Wrap( -1 );
	bSizer1711->Add( m_staticText11111, 1, wxALL|wxEXPAND, 5 );
	
	OSHeightmapPPM = new wxCheckBox( OSPanelHeightmap, wxID_ANY, wxT("PPM"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	bSizer1711->Add( OSHeightmapPPM, 0, wxALL, 5 );
	
	OSHeightmapDDS = new wxCheckBox( OSPanelHeightmap, wxID_ANY, wxT("DirectX"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	bSizer1711->Add( OSHeightmapDDS, 0, wxALL, 5 );
	
	OSHeightmapPNG = new wxCheckBox( OSPanelHeightmap, wxID_ANY, wxT("PNG"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	bSizer1711->Add( OSHeightmapPNG, 0, wxALL, 5 );
	
	bSizer14111->Add( bSizer1711, 1, wxEXPAND, 5 );
	
	OSPanelHeightmap->SetSizer( bSizer14111 );
	OSPanelHeightmap->Layout();
	bSizer14111->Fit( OSPanelHeightmap );
	bSizer38->Add( OSPanelHeightmap, 0, wxEXPAND | wxALL, 5 );
	
	OSSelectGenerator->SetSizer( bSizer38 );
	OSSelectGenerator->Layout();
	bSizer38->Fit( OSSelectGenerator );
	bSizer15->Add( OSSelectGenerator, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline711 = new wxStaticLine( OSPanelGenerator, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer15->Add( m_staticline711, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer4211;
	bSizer4211 = new wxBoxSizer( wxHORIZONTAL );
	
	OSOverwrite = new wxCheckBox( OSPanelGenerator, wxID_ANY, wxT("Overwrite"), wxDefaultPosition, wxDefaultSize, 0 );
	OSOverwrite->SetToolTip( wxT("Overwrite all files, even if they'd be valid") );
	
	bSizer4211->Add( OSOverwrite, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	OSBaseDirOut2 = new wxDirPickerCtrl( OSPanelGenerator, wxID_ANY, wxT("./generated"), wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL );
	OSBaseDirOut2->SetToolTip( wxT("The directory into which to put the generated files") );
	
	bSizer4211->Add( OSBaseDirOut2, 1, wxALL, 5 );
	
	OSHeightfieldGenerate = new wxButton( OSPanelGenerator, wxID_ANY, wxT("Generate"), wxDefaultPosition, wxDefaultSize, 0 );
	OSHeightfieldGenerate->Enable( false );
	OSHeightfieldGenerate->SetToolTip( wxT("Start generating all fiies to the given directory") );
	
	bSizer4211->Add( OSHeightfieldGenerate, 0, wxALL, 5 );
	
	bSizer15->Add( bSizer4211, 0, wxEXPAND, 5 );
	
	OSPanelGenerator->SetSizer( bSizer15 );
	OSPanelGenerator->Layout();
	bSizer15->Fit( OSPanelGenerator );
	OSToolSwitch->AddPage( OSPanelGenerator, wxT("Generator"), false );
	OSPanelInstaller = new wxPanel( OSToolSwitch, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer151;
	bSizer151 = new wxBoxSizer( wxVERTICAL );
	
	OSBaseDirIn = new wxDirPickerCtrl( OSPanelInstaller, wxID_ANY, wxT("./generated"), wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL );
	OSBaseDirIn->SetToolTip( wxT("The directory with the generated files") );
	
	bSizer151->Add( OSBaseDirIn, 0, wxALL|wxEXPAND, 5 );
	
	OSHeightfieldFirst3 = new wxStaticText( OSPanelInstaller, wxID_ANY, wxT("Select a directory first"), wxDefaultPosition, wxDefaultSize, 0 );
	OSHeightfieldFirst3->Wrap( -1 );
	OSHeightfieldFirst3->Hide();
	
	bSizer151->Add( OSHeightfieldFirst3, 1, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 25 );
	
	OSInstallWS = new wxListbook( OSPanelInstaller, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT );
	m_scrolledWindow4 = new wxScrolledWindow( OSInstallWS, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow4->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer52;
	bSizer52 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* OSLevel0;
	OSLevel0 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindow4, wxID_ANY, wxT("Level0") ), wxVERTICAL );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxHORIZONTAL );
	
	OSInstallLevel0 = new wxCheckBox( m_scrolledWindow4, wxID_LEVEL0_INST, wxT("Install"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer28->Add( OSInstallLevel0, 0, wxALL, 5 );
	
	OSInstallLevel0UVs = new wxCheckBox( m_scrolledWindow4, wxID_LEVEL0UV_INST, wxT("with UVs"), wxDefaultPosition, wxDefaultSize, 0 );
	OSInstallLevel0UVs->Hide();
	
	bSizer28->Add( OSInstallLevel0UVs, 0, wxALL, 5 );
	
	OSLevel0->Add( bSizer28, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText40 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText40->Wrap( -1 );
	fgSizer1->Add( m_staticText40, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString OSInstallLevel0MeshResChoices;
	OSInstallLevel0MeshRes = new wxChoice( m_scrolledWindow4, wxID_LEVEL0_MRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel0MeshResChoices, 0 );
	OSInstallLevel0MeshRes->SetSelection( 0 );
	fgSizer1->Add( OSInstallLevel0MeshRes, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText401 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText401->Wrap( -1 );
	fgSizer1->Add( m_staticText401, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString OSInstallLevel0TextResNChoices;
	OSInstallLevel0TextResN = new wxChoice( m_scrolledWindow4, wxID_LEVEL0N_TRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel0TextResNChoices, 0 );
	OSInstallLevel0TextResN->SetSelection( 0 );
	fgSizer1->Add( OSInstallLevel0TextResN, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4014 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4014->Wrap( -1 );
	fgSizer1->Add( m_staticText4014, 0, wxALL, 5 );
	
	wxArrayString OSInstallLevel0TextResCChoices;
	OSInstallLevel0TextResC = new wxChoice( m_scrolledWindow4, wxID_LEVEL0C_TRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel0TextResCChoices, 0 );
	OSInstallLevel0TextResC->SetSelection( 0 );
	fgSizer1->Add( OSInstallLevel0TextResC, 0, wxALL|wxEXPAND, 5 );
	
	OSLevel0->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	bSizer52->Add( OSLevel0, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* OSLevel1;
	OSLevel1 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindow4, wxID_ANY, wxT("Level1") ), wxVERTICAL );
	
	wxBoxSizer* bSizer281;
	bSizer281 = new wxBoxSizer( wxHORIZONTAL );
	
	OSInstallLevel1 = new wxCheckBox( m_scrolledWindow4, wxID_LEVEL1_INST, wxT("Install"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer281->Add( OSInstallLevel1, 0, wxALL, 5 );
	
	OSInstallLevel1UVs = new wxCheckBox( m_scrolledWindow4, wxID_LEVEL1UV_INST, wxT("with UVs"), wxDefaultPosition, wxDefaultSize, 0 );
	OSInstallLevel1UVs->Hide();
	
	bSizer281->Add( OSInstallLevel1UVs, 0, wxALL, 5 );
	
	OSLevel1->Add( bSizer281, 1, wxEXPAND, 5 );
	
	wxGridSizer* gSizer41;
	gSizer41 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_staticText402 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText402->Wrap( -1 );
	gSizer41->Add( m_staticText402, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString OSInstallLevel1MeshResChoices;
	OSInstallLevel1MeshRes = new wxChoice( m_scrolledWindow4, wxID_LEVEL1_MRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel1MeshResChoices, 0 );
	OSInstallLevel1MeshRes->SetSelection( 0 );
	gSizer41->Add( OSInstallLevel1MeshRes, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4011 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4011->Wrap( -1 );
	gSizer41->Add( m_staticText4011, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString OSInstallLevel1TextResNChoices;
	OSInstallLevel1TextResN = new wxChoice( m_scrolledWindow4, wxID_LEVEL1N_TRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel1TextResNChoices, 0 );
	OSInstallLevel1TextResN->SetSelection( 0 );
	gSizer41->Add( OSInstallLevel1TextResN, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText40111 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText40111->Wrap( -1 );
	gSizer41->Add( m_staticText40111, 0, wxALL, 5 );
	
	wxArrayString OSInstallLevel1TextResCChoices;
	OSInstallLevel1TextResC = new wxChoice( m_scrolledWindow4, wxID_LEVEL1C_TRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel1TextResCChoices, 0 );
	OSInstallLevel1TextResC->SetSelection( 0 );
	gSizer41->Add( OSInstallLevel1TextResC, 0, wxALL|wxEXPAND, 5 );
	
	OSLevel1->Add( gSizer41, 0, wxEXPAND, 5 );
	
	bSizer52->Add( OSLevel1, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* OSLevel2;
	OSLevel2 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindow4, wxID_ANY, wxT("Level2") ), wxVERTICAL );
	
	wxBoxSizer* bSizer282;
	bSizer282 = new wxBoxSizer( wxHORIZONTAL );
	
	OSInstallLevel2 = new wxCheckBox( m_scrolledWindow4, wxID_LEVEL2_INST, wxT("Install"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer282->Add( OSInstallLevel2, 0, wxALL, 5 );
	
	OSInstallLevel2UVs = new wxCheckBox( m_scrolledWindow4, wxID_LEVEL2UV_INST, wxT("with UVs"), wxDefaultPosition, wxDefaultSize, 0 );
	OSInstallLevel2UVs->Hide();
	
	bSizer282->Add( OSInstallLevel2UVs, 0, wxALL, 5 );
	
	OSLevel2->Add( bSizer282, 1, wxEXPAND, 5 );
	
	wxGridSizer* gSizer42;
	gSizer42 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_staticText403 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText403->Wrap( -1 );
	gSizer42->Add( m_staticText403, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString OSInstallLevel2MeshResChoices;
	OSInstallLevel2MeshRes = new wxChoice( m_scrolledWindow4, wxID_LEVEL2_MRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel2MeshResChoices, 0 );
	OSInstallLevel2MeshRes->SetSelection( 0 );
	gSizer42->Add( OSInstallLevel2MeshRes, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4012 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4012->Wrap( -1 );
	gSizer42->Add( m_staticText4012, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString OSInstallLevel2TextResNChoices;
	OSInstallLevel2TextResN = new wxChoice( m_scrolledWindow4, wxID_LEVEL2N_TRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel2TextResNChoices, 0 );
	OSInstallLevel2TextResN->SetSelection( 0 );
	gSizer42->Add( OSInstallLevel2TextResN, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText40121 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText40121->Wrap( -1 );
	gSizer42->Add( m_staticText40121, 0, wxALL, 5 );
	
	wxArrayString OSInstallLevel2TextResCChoices;
	OSInstallLevel2TextResC = new wxChoice( m_scrolledWindow4, wxID_LEVEL2C_TRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel2TextResCChoices, 0 );
	OSInstallLevel2TextResC->SetSelection( 0 );
	gSizer42->Add( OSInstallLevel2TextResC, 0, wxALL|wxEXPAND, 5 );
	
	OSLevel2->Add( gSizer42, 0, wxEXPAND, 5 );
	
	bSizer52->Add( OSLevel2, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* OSLevel3;
	OSLevel3 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindow4, wxID_ANY, wxT("Level3") ), wxVERTICAL );
	
	wxBoxSizer* bSizer283;
	bSizer283 = new wxBoxSizer( wxHORIZONTAL );
	
	OSInstallLevel3 = new wxCheckBox( m_scrolledWindow4, wxID_LEVEL3_INST, wxT("Install"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer283->Add( OSInstallLevel3, 0, wxALL, 5 );
	
	OSInstallLevel3UVs = new wxCheckBox( m_scrolledWindow4, wxID_LEVEL3UV_INST, wxT("with UVs"), wxDefaultPosition, wxDefaultSize, 0 );
	OSInstallLevel3UVs->Hide();
	
	bSizer283->Add( OSInstallLevel3UVs, 0, wxALL, 5 );
	
	OSLevel3->Add( bSizer283, 1, wxEXPAND, 5 );
	
	wxGridSizer* gSizer43;
	gSizer43 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_staticText404 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Mesh-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText404->Wrap( -1 );
	gSizer43->Add( m_staticText404, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString OSInstallLevel3MeshResChoices;
	OSInstallLevel3MeshRes = new wxChoice( m_scrolledWindow4, wxID_LEVEL3_MRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel3MeshResChoices, 0 );
	OSInstallLevel3MeshRes->SetSelection( 0 );
	gSizer43->Add( OSInstallLevel3MeshRes, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4013 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxT("Texture-resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4013->Wrap( -1 );
	gSizer43->Add( m_staticText4013, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString OSInstallLevel3TextResNChoices;
	OSInstallLevel3TextResN = new wxChoice( m_scrolledWindow4, wxID_LEVEL3N_TRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel3TextResNChoices, 0 );
	OSInstallLevel3TextResN->SetSelection( 0 );
	gSizer43->Add( OSInstallLevel3TextResN, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText40131 = new wxStaticText( m_scrolledWindow4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText40131->Wrap( -1 );
	gSizer43->Add( m_staticText40131, 0, wxALL, 5 );
	
	wxArrayString OSInstallLevel3TextResCChoices;
	OSInstallLevel3TextResC = new wxChoice( m_scrolledWindow4, wxID_LEVEL3C_TRES, wxDefaultPosition, wxDefaultSize, OSInstallLevel3TextResCChoices, 0 );
	OSInstallLevel3TextResC->SetSelection( 0 );
	gSizer43->Add( OSInstallLevel3TextResC, 0, wxALL|wxEXPAND, 5 );
	
	OSLevel3->Add( gSizer43, 0, wxEXPAND, 5 );
	
	bSizer52->Add( OSLevel3, 0, wxEXPAND, 5 );
	
	m_scrolledWindow4->SetSizer( bSizer52 );
	m_scrolledWindow4->Layout();
	bSizer52->Fit( m_scrolledWindow4 );
	OSInstallWS->AddPage( m_scrolledWindow4, wxT("Tamriel"), false );
	#ifndef __WXGTK__ // Small icon style not supported in GTK
	wxListView* OSInstallWSListView = OSInstallWS->GetListView();
	long OSInstallWSFlags = OSInstallWSListView->GetWindowStyleFlag();
	OSInstallWSFlags = ( OSInstallWSFlags & ~wxLC_ICON ) | wxLC_SMALL_ICON;
	OSInstallWSListView->SetWindowStyleFlag( OSInstallWSFlags );
	#endif
	
	bSizer151->Add( OSInstallWS, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline7111 = new wxStaticLine( OSPanelInstaller, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer151->Add( m_staticline7111, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer42111;
	bSizer42111 = new wxBoxSizer( wxHORIZONTAL );
	
	OSPlugoutDir = new wxDirPickerCtrl( OSPanelInstaller, wxID_ANY, wxT("./Data"), wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL );
	OSPlugoutDir->SetToolTip( wxT("The directory to install to, can be the \"Data\"-folder or any other location") );
	
	bSizer42111->Add( OSPlugoutDir, 1, wxALL, 5 );
	
	OSHeightfieldInstall = new wxButton( OSPanelInstaller, wxID_ANY, wxT("Install"), wxDefaultPosition, wxDefaultSize, 0 );
	OSHeightfieldInstall->Enable( false );
	OSHeightfieldInstall->SetToolTip( wxT("Start installing the desired files to the given directory") );
	
	bSizer42111->Add( OSHeightfieldInstall, 0, wxALL, 5 );
	
	bSizer151->Add( bSizer42111, 0, wxEXPAND, 5 );
	
	OSPanelInstaller->SetSizer( bSizer151 );
	OSPanelInstaller->Layout();
	bSizer151->Fit( OSPanelInstaller );
	OSToolSwitch->AddPage( OSPanelInstaller, wxT("Installer"), false );
	OSPanelReverse = new wxPanel( OSToolSwitch, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer391;
	bSizer391 = new wxBoxSizer( wxVERTICAL );
	
	OSLODDir = new wxDirPickerCtrl( OSPanelReverse, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL );
	OSLODDir->SetToolTip( wxT("The directory with the LOD-files. Most often the \"Data/Meshes/Terrain\"-folder.") );
	
	bSizer391->Add( OSLODDir, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer321;
	bSizer321 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText311 = new wxStaticText( OSPanelReverse, wxID_ANY, wxT("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText311->Wrap( -1 );
	bSizer321->Add( m_staticText311, 1, wxALL, 5 );
	
	OSRevHeight = new wxRadioButton( OSPanelReverse, wxID_ANY, wxT("heightfield"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	OSRevHeight->SetValue( true ); 
	OSRevHeight->SetToolTip( wxT("Recover a heightfield from the files in the given directory") );
	
	bSizer321->Add( OSRevHeight, 0, wxALL, 5 );
	
	OSRevNormals = new wxRadioButton( OSPanelReverse, wxID_ANY, wxT("normal-map"), wxDefaultPosition, wxDefaultSize, 0 );
	OSRevNormals->Hide();
	OSRevNormals->SetToolTip( wxT("Recover a normal-map from the files in the given directory") );
	
	bSizer321->Add( OSRevNormals, 0, wxALL, 5 );
	
	OSRevColors = new wxRadioButton( OSPanelReverse, wxID_ANY, wxT("color-map"), wxDefaultPosition, wxDefaultSize, 0 );
	OSRevColors->SetToolTip( wxT("Recover a surface-map from the files in the given directory") );
	
	bSizer321->Add( OSRevColors, 0, wxALL, 5 );
	
	bSizer391->Add( bSizer321, 0, wxEXPAND, 5 );
	
	wxString OSLODWorldspaceChoices[] = { wxT("Tamriel"), wxT("SEWorld") };
	int OSLODWorldspaceNChoices = sizeof( OSLODWorldspaceChoices ) / sizeof( wxString );
	OSLODWorldspace = new wxChoice( OSPanelReverse, wxID_ANY, wxDefaultPosition, wxDefaultSize, OSLODWorldspaceNChoices, OSLODWorldspaceChoices, 0 );
	OSLODWorldspace->SetSelection( 0 );
	OSLODWorldspace->SetToolTip( wxT("Filter files for the worldspace defined here") );
	
	bSizer391->Add( OSLODWorldspace, 0, wxALL|wxEXPAND, 5 );
	
	wxString OSLODResolutionChoices[] = { wxT("From 4x4 Cells"), wxT("From 8x8 Cells"), wxT("From 16x16 Cells"), wxT("From 32x32 Cells") };
	int OSLODResolutionNChoices = sizeof( OSLODResolutionChoices ) / sizeof( wxString );
	OSLODResolution = new wxChoice( OSPanelReverse, wxID_ANY, wxDefaultPosition, wxDefaultSize, OSLODResolutionNChoices, OSLODResolutionChoices, 0 );
	OSLODResolution->SetSelection( 0 );
	OSLODResolution->SetToolTip( wxT("Filter files for the resolution defined here") );
	
	bSizer391->Add( OSLODResolution, 0, wxALL|wxEXPAND, 5 );
	
	wxString OSLODListChoices[] = { wxT("a.btr"), wxT("b.btr") };
	int OSLODListNChoices = sizeof( OSLODListChoices ) / sizeof( wxString );
	OSLODList = new wxCheckListBox( OSPanelReverse, wxID_ANY, wxDefaultPosition, wxDefaultSize, OSLODListNChoices, OSLODListChoices, 0 );
	OSLODList->SetToolTip( wxT("The LODs which should be used to define the recovery rectangle") );
	
	bSizer391->Add( OSLODList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer422;
	bSizer422 = new wxBoxSizer( wxHORIZONTAL );
	
	OSFileRecoveryOut = new wxFilePickerCtrl( OSPanelReverse, wxID_ANY, wxT("./Tamriel.raw"), wxT("Select a file"), wxT("Raw Oscape files (*.raw;*.land)|*.raw;*.land"), wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL );
	OSFileRecoveryOut->SetToolTip( wxT("The file to recover to, the extension is automatically set (.raw for heightfields, .land for surface-maps)") );
	
	bSizer422->Add( OSFileRecoveryOut, 1, wxALL, 5 );
	
	OSLODRecover = new wxButton( OSPanelReverse, wxID_ANY, wxT("Recover"), wxDefaultPosition, wxDefaultSize, 0 );
	OSLODRecover->Enable( false );
	OSLODRecover->SetToolTip( wxT("Start recovering the desired data to the given file") );
	
	bSizer422->Add( OSLODRecover, 0, wxALL, 5 );
	
	bSizer391->Add( bSizer422, 0, wxEXPAND, 5 );
	
	OSPanelReverse->SetSizer( bSizer391 );
	OSPanelReverse->Layout();
	bSizer391->Fit( OSPanelReverse );
	OSToolSwitch->AddPage( OSPanelReverse, wxT("Recovery"), false );
	
	bSizer1->Add( OSToolSwitch, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	OSStatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( OSOblivion->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeToOblivion ) );
	this->Connect( OSSkyrim->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeToSkyrim ) );
	this->Connect( OSDefaultVanilla->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeDefaults ) );
	this->Connect( OSDefaultTWMP->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeDefaults ) );
	this->Connect( OSDefaultLLOD->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeDefaults ) );
	this->Connect( OSTS1x1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeTSampling ) );
	this->Connect( OSTS2x2->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeTSampling ) );
	this->Connect( OSTS4x4->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeTSampling ) );
	this->Connect( OSS1x->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeSSampling ) );
	this->Connect( OSS2x->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeSSampling ) );
	this->Connect( OSS4x->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeSSampling ) );
	this->Connect( wxID_RESET, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxOscape::ResetPluginList ) );
	this->Connect( wxID_CLEAR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxOscape::ClearPluginList ) );
	this->Connect( wxID_LOAD, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxOscape::LoadPluginList ) );
	this->Connect( wxID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxOscape::SavePluginList ) );
	OSPluginAutosave->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeAutosave ), NULL, this );
	OSPluginDir->Connect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangePluginDir ), NULL, this );
	OSPluginList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( wxOscape::ChangeActivePlugins ), NULL, this );
	OSPluginList->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( wxOscape::ChangeActivePlugins ), NULL, this );
	OSWorldspace->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( wxOscape::ChangeWorldspace ), NULL, this );
	OSWorldspaceFill->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::WorldspacesFromPlugins ), NULL, this );
	OSFileHeightfieldOut->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeHeightfieldOut ), NULL, this );
	OSPluginExtract->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::HeightfieldExtract ), NULL, this );
	OSFileHeightfieldIn1->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeHeightfieldIn1 ), NULL, this );
	OSPreviewSelector->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangePreview ), NULL, this );
	OSPreview->Connect( wxEVT_PAINT, wxPaintEventHandler( wxOscape::PaintH ), NULL, this );
	OSOrientation->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeOrientation ), NULL, this );
	OSFilePoints1->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangePointsIn1 ), NULL, this );
	OSPointsClear1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::ClearPoints1 ), NULL, this );
	OSBaseDirOut1->Connect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeBaseDirOut1 ), NULL, this );
	OSHeightfieldAccept->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::HeightfieldAccept ), NULL, this );
	OSFileHeightfieldIn2->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeHeightfieldIn2 ), NULL, this );
	OSFilePoints2->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangePointsIn2 ), NULL, this );
	OSPointsClear2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::ClearPoints2 ), NULL, this );
	OSAlgorithm->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeAlgorithm ), NULL, this );
	OSQThreshold->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxOscape::CheckFloat ), NULL, this );
	OSAThreshold->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxOscape::CheckFloat ), NULL, this );
	OSEmphasis->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxOscape::CheckFloat ), NULL, this );
	OSTermination->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxOscape::CheckFloat ), NULL, this );
	OSTarget->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( wxOscape::ChangeTarget ), NULL, this );
	OSTarget->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxOscape::CheckInt ), NULL, this );
	OSMeshes->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeMeshes ), NULL, this );
	OSNormals->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeNormals ), NULL, this );
	OSColors->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeColors ), NULL, this );
	OSHeightmap->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeHeightmap ), NULL, this );
	OSBaseDirOut2->Connect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeBaseDirOut2 ), NULL, this );
	OSHeightfieldGenerate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::HeightfieldGenerate ), NULL, this );
	OSBaseDirIn->Connect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeBaseDirIn ), NULL, this );
	OSInstallLevel0MeshRes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLevel0MeshRes ), NULL, this );
	OSInstallLevel1MeshRes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLevel1MeshRes ), NULL, this );
	OSInstallLevel2MeshRes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLevel2MeshRes ), NULL, this );
	OSInstallLevel3MeshRes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLevel3MeshRes ), NULL, this );
	OSPlugoutDir->Connect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangePlugoutDir ), NULL, this );
	OSHeightfieldInstall->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::HeightfieldInstall ), NULL, this );
	OSLODDir->Connect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeLODDir ), NULL, this );
	OSRevHeight->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( wxOscape::ChangeToHeightfield ), NULL, this );
	OSRevNormals->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( wxOscape::ChangeToNormalmap ), NULL, this );
	OSRevColors->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( wxOscape::ChangeToColormap ), NULL, this );
	OSLODWorldspace->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLODWorldspace ), NULL, this );
	OSLODResolution->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLODRes ), NULL, this );
	OSLODList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( wxOscape::ChangeActiveLODs ), NULL, this );
	OSLODList->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( wxOscape::ChangeActiveLODs ), NULL, this );
	OSFileRecoveryOut->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeRecoveryOut ), NULL, this );
	OSLODRecover->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::HeightfieldRecover ), NULL, this );
}

wxOscape::~wxOscape()
{
	// Disconnect Events
	this->Disconnect( wxID_OBLIVON, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeToOblivion ) );
	this->Disconnect( wxID_SKYRIM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeToSkyrim ) );
	this->Disconnect( wxID_DEFV, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeDefaults ) );
	this->Disconnect( wxID_DEFT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeDefaults ) );
	this->Disconnect( wxID_DEFL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeDefaults ) );
	this->Disconnect( wxID_TS1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeTSampling ) );
	this->Disconnect( wxID_TS2, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeTSampling ) );
	this->Disconnect( wxID_TS4, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeTSampling ) );
	this->Disconnect( wxID_SS1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeSSampling ) );
	this->Disconnect( wxID_SS2, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeSSampling ) );
	this->Disconnect( wxID_SS4, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxOscape::ChangeSSampling ) );
	this->Disconnect( wxID_RESET, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxOscape::ResetPluginList ) );
	this->Disconnect( wxID_CLEAR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxOscape::ClearPluginList ) );
	this->Disconnect( wxID_LOAD, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxOscape::LoadPluginList ) );
	this->Disconnect( wxID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxOscape::SavePluginList ) );
	OSPluginAutosave->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeAutosave ), NULL, this );
	OSPluginDir->Disconnect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangePluginDir ), NULL, this );
	OSPluginList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( wxOscape::ChangeActivePlugins ), NULL, this );
	OSPluginList->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( wxOscape::ChangeActivePlugins ), NULL, this );
	OSWorldspace->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( wxOscape::ChangeWorldspace ), NULL, this );
	OSWorldspaceFill->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::WorldspacesFromPlugins ), NULL, this );
	OSFileHeightfieldOut->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeHeightfieldOut ), NULL, this );
	OSPluginExtract->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::HeightfieldExtract ), NULL, this );
	OSFileHeightfieldIn1->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeHeightfieldIn1 ), NULL, this );
	OSPreviewSelector->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangePreview ), NULL, this );
	OSPreview->Disconnect( wxEVT_PAINT, wxPaintEventHandler( wxOscape::PaintH ), NULL, this );
	OSOrientation->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeOrientation ), NULL, this );
	OSFilePoints1->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangePointsIn1 ), NULL, this );
	OSPointsClear1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::ClearPoints1 ), NULL, this );
	OSBaseDirOut1->Disconnect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeBaseDirOut1 ), NULL, this );
	OSHeightfieldAccept->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::HeightfieldAccept ), NULL, this );
	OSFileHeightfieldIn2->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeHeightfieldIn2 ), NULL, this );
	OSFilePoints2->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangePointsIn2 ), NULL, this );
	OSPointsClear2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::ClearPoints2 ), NULL, this );
	OSAlgorithm->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeAlgorithm ), NULL, this );
	OSQThreshold->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxOscape::CheckFloat ), NULL, this );
	OSAThreshold->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxOscape::CheckFloat ), NULL, this );
	OSEmphasis->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxOscape::CheckFloat ), NULL, this );
	OSTermination->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxOscape::CheckFloat ), NULL, this );
	OSTarget->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( wxOscape::ChangeTarget ), NULL, this );
	OSTarget->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( wxOscape::CheckInt ), NULL, this );
	OSMeshes->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeMeshes ), NULL, this );
	OSNormals->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeNormals ), NULL, this );
	OSColors->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeColors ), NULL, this );
	OSHeightmap->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( wxOscape::ChangeHeightmap ), NULL, this );
	OSBaseDirOut2->Disconnect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeBaseDirOut2 ), NULL, this );
	OSHeightfieldGenerate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::HeightfieldGenerate ), NULL, this );
	OSBaseDirIn->Disconnect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeBaseDirIn ), NULL, this );
	OSInstallLevel0MeshRes->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLevel0MeshRes ), NULL, this );
	OSInstallLevel1MeshRes->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLevel1MeshRes ), NULL, this );
	OSInstallLevel2MeshRes->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLevel2MeshRes ), NULL, this );
	OSInstallLevel3MeshRes->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLevel3MeshRes ), NULL, this );
	OSPlugoutDir->Disconnect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangePlugoutDir ), NULL, this );
	OSHeightfieldInstall->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::HeightfieldInstall ), NULL, this );
	OSLODDir->Disconnect( wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeLODDir ), NULL, this );
	OSRevHeight->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( wxOscape::ChangeToHeightfield ), NULL, this );
	OSRevNormals->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( wxOscape::ChangeToNormalmap ), NULL, this );
	OSRevColors->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( wxOscape::ChangeToColormap ), NULL, this );
	OSLODWorldspace->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLODWorldspace ), NULL, this );
	OSLODResolution->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( wxOscape::ChangeLODRes ), NULL, this );
	OSLODList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( wxOscape::ChangeActiveLODs ), NULL, this );
	OSLODList->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( wxOscape::ChangeActiveLODs ), NULL, this );
	OSFileRecoveryOut->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( wxOscape::ChangeRecoveryOut ), NULL, this );
	OSLODRecover->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxOscape::HeightfieldRecover ), NULL, this );
	
}

wxProgress::wxProgress( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,265 ), wxDefaultSize );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxVERTICAL );
	
	m_panel9 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	OSSubject1 = new wxStaticText( m_panel9, wxID_ANY, wxT("Mesh resolution X:"), wxDefaultPosition, wxDefaultSize, 0 );
	OSSubject1->Wrap( -1 );
	bSizer26->Add( OSSubject1, 0, wxALL, 5 );
	
	OSTask1 = new wxGauge( m_panel9, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,25 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	bSizer26->Add( OSTask1, 0, wxALL|wxEXPAND, 5 );
	
	OSSubject2 = new wxStaticText( m_panel9, wxID_ANY, wxT("Line Y:"), wxDefaultPosition, wxDefaultSize, 0 );
	OSSubject2->Wrap( -1 );
	bSizer26->Add( OSSubject2, 0, wxALL, 5 );
	
	OSTask2 = new wxGauge( m_panel9, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,25 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	bSizer26->Add( OSTask2, 0, wxALL|wxEXPAND, 5 );
	
	wxGridSizer* gSizer6;
	gSizer6 = new wxGridSizer( 2, 2, 0, 25 );
	
	m_staticText25 = new wxStaticText( m_panel9, wxID_ANY, wxT("Running:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	gSizer6->Add( m_staticText25, 0, wxALL, 2 );
	
	OSRunning = new wxStaticText( m_panel9, wxID_ANY, wxT("00:00s"), wxDefaultPosition, wxDefaultSize, 0 );
	OSRunning->Wrap( -1 );
	gSizer6->Add( OSRunning, 0, wxALIGN_RIGHT|wxALL, 2 );
	
	m_staticText27 = new wxStaticText( m_panel9, wxID_ANY, wxT("Remaining:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	gSizer6->Add( m_staticText27, 0, wxALL, 2 );
	
	OSRemaining = new wxStaticText( m_panel9, wxID_ANY, wxT("00:00s"), wxDefaultPosition, wxDefaultSize, 0 );
	OSRemaining->Wrap( -1 );
	gSizer6->Add( OSRemaining, 0, wxALIGN_RIGHT|wxALL, 2 );
	
	m_staticText29 = new wxStaticText( m_panel9, wxID_ANY, wxT("Total:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	gSizer6->Add( m_staticText29, 0, wxALL, 2 );
	
	OSTotal = new wxStaticText( m_panel9, wxID_ANY, wxT("00:00s"), wxDefaultPosition, wxDefaultSize, 0 );
	OSTotal->Wrap( -1 );
	gSizer6->Add( OSTotal, 0, wxALIGN_RIGHT|wxALL, 1 );
	
	bSizer26->Add( gSizer6, 0, wxALIGN_CENTER, 5 );
	
	m_staticline7 = new wxStaticLine( m_panel9, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer26->Add( m_staticline7, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxHORIZONTAL );
	
	OSPause = new wxButton( m_panel9, wxID_ANY, wxT("Pause"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer27->Add( OSPause, 0, wxALL, 5 );
	
	OSAbort = new wxButton( m_panel9, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer27->Add( OSAbort, 0, wxALL, 5 );
	
	bSizer26->Add( bSizer27, 0, wxALIGN_CENTER, 5 );
	
	m_panel9->SetSizer( bSizer26 );
	m_panel9->Layout();
	bSizer26->Fit( m_panel9 );
	bSizer25->Add( m_panel9, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer25 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( wxProgress::AbortProgress ) );
	this->Connect( wxEVT_IDLE, wxIdleEventHandler( wxProgress::IdleProgress ) );
	OSPause->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxProgress::PauseProgress ), NULL, this );
	OSAbort->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxProgress::AbortProgress ), NULL, this );
}

wxProgress::~wxProgress()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( wxProgress::AbortProgress ) );
	this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxProgress::IdleProgress ) );
	OSPause->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxProgress::PauseProgress ), NULL, this );
	OSAbort->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxProgress::AbortProgress ), NULL, this );
	
}
