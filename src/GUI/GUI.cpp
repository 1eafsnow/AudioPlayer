#include "GUI.h"
#include "Input.h"

void AudioSourceWindow::Render()
{

}

void TrackItem::Render()
{

}

void TrackListWindow::Render()
{

}

GUI::GUI()
{
    
}

GUI::~GUI()
{

}

void GUI::DrawPixels(unsigned char* pixels)
{
    //glDrawPixels(pixelWidth, pixelHeight, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)pixels);
}

void GUI::CreateAudioSource(Vector3F location)
{
	AudioSource* source = audioPlayer->CreateSource();
    source->SetPosition(location);

    audioSourceWindows.push_back(AudioSourceWindow());
    AudioSourceWindow& window = audioSourceWindows.back();
    window.id = source->sid;
    window.source = source;
    _itoa(source->sid, window.label + 6, 10);

    window.mesh = renderer->CreateSourceMesh(location, 0.1f, 16);

    //ImGui::PushID(window.id);
    //ImGui::DockBuilderDockWindow(window.label, leftDownNode);
    //ImGui::PopID();
    ImGui::DockBuilderRemoveNode(dockspaceID);

    printf("Create source%d window\n", source->sid);
}

void GUI::DestoryAudioSource(int sourceId)
{
	auto window = std::find_if(audioSourceWindows.begin(), audioSourceWindows.end(), [sourceId](AudioSourceWindow& w) { return w.source->sid == sourceId; });
	int sid = window->source->sid;
    audioPlayer->DestorySource(window->source);
    audioSourceWindows.erase(window);
    printf("Destory source%d window\n", sid);
}

void GUI::CreateTrackList(const char* name)
{
    auto trackListIt = audioPlayer->CreateTrackList(name);
    trackListWindows.push_back(TrackListWindow());
    auto& window = trackListWindows.back();
    window.id = trackListIt->id;
    window.trackListIt = trackListIt;
    printf("Create TrackList: %d - %s\n", trackListIt->id, trackListIt->name.c_str());
}

void GUI::AddTrack(int trackListID, const char* path)
{
    /*
    Wave wav;
	wav.Load(path);
    Wave* track = new Wave;
    StereoToMono(wav, *track);
    */
    Wave* track = new Wave;
    track->Load(path);

    auto window = std::find_if(trackListWindows.begin(), trackListWindows.end(), [trackListID](TrackListWindow& w) {return w.id == trackListID; });
    if (window == trackListWindows.end())
    {
        printf("TrackList ID: %d not found\n", trackListID);
        return;
    }
    auto at = window->trackListIt->AddTrack(track);
    //it->trackListIt = window->trackListIt;
    window->trackItems.push_back({ at->id, at , false});
    printf("TrackList: %s, Add track: %d - %s\n", window->trackListIt->name, at->id, at->source->path.c_str());
}

void GUI::AddTrack(int trackListID, TrackItem* track)
{
    auto window = std::find_if(trackListWindows.begin(), trackListWindows.end(), [trackListID](TrackListWindow& w) {return w.id == trackListID; });
    if (window == trackListWindows.end())
    {
        printf("TrackList ID: %d not found\n", trackListID);
        return;
    }
    auto at = window->trackListIt->AddTrack(track->track->source);
    //it->trackListIt = window->trackListIt;
    window->trackItems.push_back({ at->id, at , false });
    printf("TrackList: %s, Add track: %d - %s\n", window->trackListIt->name, at->id, at->source->path.c_str());
}

void GUI::ShowAudioSourceWindow()
{
    for (auto& window : audioSourceWindows)
    {
        if (!window.show)
        {
            DestoryAudioSource(window.source->sid);
            continue;
        }
        AudioSource* source = window.source;
        window.mesh->SetWorldLocation(Vector3F(source->position[0], source->position[1], source->position[2]));

        ImGui::PushID(window.id);
        //ImGui::SetNextWindowPos(ImVec2(0, 0));
        //ImGui::SetNextWindowSize(ImVec2(800, 300));
        ImGui::Begin(window.label, &window.show, window.windowFlags);
        window.isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        if (Wave* track = source->GetPlayingTrackSource())
        {
            ImGui::Text(u8"Track: %s", track->nameU8.c_str());
            //ImGui::Text(track->pathU8.c_str());
        }
        else
        {
            ImGui::Text(u8"Track: null");
        }
        
        ImGui::SetNextItemWidth(785);
        ImGui::SliderFloat("##progress", &window.progress, 0.0f, 1.0f);
        if (ImGui::IsItemDeactivated())
        {
            source->SetProgress(window.progress);
        }
        if (!ImGui::IsItemActive())
        {
            window.progress = source->GetProgress();
        }
        ImGui::SameLine(); 
        float totalDuration = source->GetPlayingTrackTotalDuration();
        float processedDuration = source->GetPlayingTrackProcessedDuration();
        ImGui::Text("%dm%ds / %dm%ds", (uint32_t)processedDuration / 60, (uint32_t)processedDuration % 60, (uint32_t)totalDuration / 60, (uint32_t)totalDuration % 60);

        ImGui::Button("Previous", ImVec2(70, 25));
        if (ImGui::IsItemDeactivated())
        {
            //source.PlayPrevious();
        }

        ImGui::SameLine();
        if (source->state != AL_PLAYING)
        {
            ImGui::Button("Play", ImVec2(70, 25));
            if (ImGui::IsItemDeactivated())
            {
                if (source->state == AL_STOPPED || source->state == AL_INITIAL)
                {
                    //source->Play(nullptr);
                }
                else if (source->state == AL_PAUSED)
                {
                    source->Resume();
                }
            }
        }
        else
        {
            ImGui::Button("Pause", ImVec2(70, 25));
            if (ImGui::IsItemDeactivated())
            {
                source->Pause();
            }
        }

        ImGui::SameLine();
        ImGui::Button("Stop", ImVec2(70, 25));
        if (ImGui::IsItemDeactivated())
        {
            source->Stop();
        }

        ImGui::SameLine();
        ImGui::Button("Next", ImVec2(70, 25));
        if (ImGui::IsItemDeactivated())
        {
            source->PlayNext();
        }

        if (ImGui::Combo("LoopType", (int*)&source->loopType, "TrackOnce\0TrackLoop\0ListOnce\0ListLoop\0ListRandom\0"))
        {
            /*
            switch (source->loopType)
            {
            case 0: ImGui::StyleColorsDark(); break;
            case 1: ImGui::StyleColorsLight(); break;
            case 2: ImGui::StyleColorsClassic(); break;
            }
            */
        }

        ImGui::InputFloat3("Position", source->position, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
        if (ImGui::IsItemDeactivated())
        {
            //source->UpdatePosition();
        }

        ImGui::InputFloat3("Direction", source->direction, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
        if (ImGui::IsItemDeactivated())
        {
            //source->UpdateDirection();
        }

        ImGui::InputFloat3("Velocity", source->velocity, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
        if (ImGui::IsItemDeactivated())
        {
            //source->UpdateVelocity();
        }

        ImGui::SetNextItemWidth(200);
        ImGui::SliderFloat("Volume", &source->volume, 0.0f, 1.0f, "%.2f");
        if (ImGui::IsItemActive())
        {
            source->UpdateVolume();
        }        

        ImGui::End();
        ImGui::PopID();
    }    
}

void GUI::ShowListenerWindow()
{
    if (showListener)
    {
        //ImGui::SetNextWindowPos(ImVec2(0, 600));
        //ImGui::SetNextWindowSize(ImVec2(1200, 300));
        ImGui::Begin("Listener", &showListener);                          // Create a window called "Hello, world!" and append into it.

        //ImGui::Checkbox("Demo GUI", &show_demo_window);      // Edit bools storing our window open/close state
        //ImGui::Checkbox("Another GUI", &show_another_window);    

        ImGui::InputFloat3("ListenerPosition", audioPlayer->listenerPosition, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
        if (ImGui::IsItemDeactivated())
        {
            //audioPlayer->UpdateListenerPosition();
        }

        ImGui::InputFloat3("ListenerRotation", audioPlayer->listenerRotation, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
        if (ImGui::IsItemDeactivated())
        {
            //audioPlayer->UpdateListenerRotation();
        }

        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }
}

void GUI::ShowTrackListWindow()
{
    for (auto& window : trackListWindows)
    {
        if (!window.show)
        {
            return;
        }
        ImGui::PushID(window.id);
        //ImGui::DockSpaceOverViewport(2, ImGui::GetMainViewport());
        //ImGui::SetNextWindowPos(ImVec2(1200, 0));
        //ImGui::SetNextWindowSize(ImVec2(400, 900));
        ImGui::Begin(window.trackListIt->name.c_str(), &window.show, ImGuiWindowFlags_MenuBar);
        window.isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        for (auto& trackItem : window.trackItems)
        {
            ShowTrackItem(window, trackItem);
        }
        ImGui::End();
        ImGui::PopID();
    }
}

void GUI::ShowTrackItem(TrackListWindow& trackListWindow, TrackItem& trackItem)
{
    AudioTrackList& trackList = *(trackListWindow.trackListIt);
    AudioTrack* track = trackItem.track;
    ImGui::PushID(track->id);
    ImGui::Selectable(track->source->nameU8.c_str(), &(trackItem.isSelected));
    if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
    {
        //ImGui::Text("This a popup!");
        if (ImGui::Button("Edit"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::SetItemTooltip(u8"ID: %d\nList: %s\nPath: %s\nChannels: %u\nSampleRate: %u\nSampleSize: %u\nBitsPerSample: %u\nByteSize: %u\nDuration: %dm%ds\n",
        track->id,
        track->trackList->nameU8.c_str(),
        track->source->pathU8.c_str(),
        track->source->numChannels,
        track->source->sampleRate,
        track->source->sampleSize,
        track->source->bitsPerSample,
        track->source->byteSize,
        (uint32_t)track->source->duration / 60, (uint32_t)track->source->duration % 60);

    // Our buttons are both drag sources and drag targets here!
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        // Set payload to carry the index of our item (could be anything)
        ImGui::SetDragDropPayload("Track", &track->id, sizeof(int));

        // Display preview (could be anything, e.g. when dragging an image we could decide to display
        // the filename and a small preview of the image, etc.)
        ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Track"))
        {
            IM_ASSERT(payload->DataSize == sizeof(int));
            int payload_n = *(const int*)payload->Data;
            //printf("drag: %s, drop: %s\n", (trackList.FindTrackByID(payload_n))->name.c_str(), (*audioPlayer->FindTrackByID(track->id))->name.c_str());
            trackList.SwapTrack(payload_n, track->id);
        }
        ImGui::EndDragDropTarget();
    }    

    if (ImGui::IsItemDeactivated())
    {
        for (auto& window : audioSourceWindows)
        {
            if (window.isHovered)
            {
                window.source->Stop();
                window.source->Play(trackItem.track);
            }
        }

        for (auto& window : trackListWindows)
        {
            if (window.id == trackListWindow.id)
            {
                continue;
			}
            if (window.isHovered)
            {
				AddTrack(window.id, &trackItem);
            }
        }
    }
    ImGui::PopID();
}

void GUI::Open()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    glWindow = glfwCreateWindow((int)(width * main_scale), (int)(height * main_scale), "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (glWindow == nullptr)
    {

    }
    glfwMakeContextCurrent(glWindow);
    glfwSwapInterval(1); // Enable vsync
    //static bool opt_fullscreen = false;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    io.ConfigDockingWithShift = true;
    io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    /*
    const GLubyte* rendererer = glGetString(GL_RENDERER); // get rendererer string
    const GLubyte* version = glGetString(GL_VERSION);   // version as a string
    printf("Renderer: %s\n", rendererer);
    printf("OpenGL version supported %s\n", version);
    */
    glEnable(GL_DEPTH_TEST);
    glewInit();

    renderer = new Renderer;
    
    WaveMesh* waveMesh = new WaveMesh(512);
    renderer->meshes.push_back(waveMesh);
    
    renderer->InitBindings();
    glViewport(0, 300, renderer->width, renderer->height);
    glRasterPos2f(-1, -1);
    
    input = new Input;
    input->SetGUI(this); 

    showMainWindow = true;
    showListener = true;

}

void GUI::Close()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(glWindow);
    glfwTerminate();
}

void GUI::Tick(float deltaTime)
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    
    glfwPollEvents();
    if (glfwGetWindowAttrib(glWindow, GLFW_ICONIFIED) != 0)
    {
        ImGui_ImplGlfw_Sleep(10);
        return;
    }
    
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();    

    //ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    /*
    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
    }
    */
    ImGui::ShowDemoWindow();

    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::Begin("AudioPlayer", &showMainWindow, ImGuiWindowFlags_MenuBar);
    dockspaceID = ImGui::GetID("MainDockspace");
    if (!ImGui::DockBuilderGetNode(dockspaceID))
    {
        //ImGui::SetWindowSize(ImVec2(370.0f, 680.0f));
        //ImGui::SetWindowPos(ImVec2(640.0f, 55.0f));

        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID);
        
        ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.8f, &leftNode, &rightNode);
        ImGui::DockBuilderSplitNode(leftNode, ImGuiDir_Up, 0.7f, &leftUpNode, &leftDownNode);
        ImGui::DockBuilderDockWindow("Wave", leftUpNode);
        ImGui::DockBuilderDockWindow("Spectrum", leftUpNode);

        for (auto& window : audioSourceWindows)
        {
            ImGui::PushID(window.id);
            ImGui::DockBuilderDockWindow(window.label, leftDownNode);
            ImGui::PopID();
        }

        ImGui::DockBuilderDockWindow("Listener", leftDownNode);
        for (auto& window : trackListWindows)
        {
            ImGui::PushID(window.id);
            ImGui::DockBuilderDockWindow(window.trackListIt->name.c_str(), rightNode);
            ImGui::PopID();
        }
        ImGui::DockBuilderFinish(dockspaceID);
    }
    ImGui::DockSpace(dockspaceID);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Player"))
        {          
            ImGui::Button("ResetDocking");
            if (ImGui::IsItemDeactivated())
            {
                ImGui::DockBuilderRemoveNode(dockspaceID);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Source"))
        {
            ImGui::Button("AddSource");
            if (ImGui::IsItemDeactivated())
            {
                CreateAudioSource(Vector3F(0.0, 0.0, -5.0));
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("TrackList"))
        {
            ImGui::Button("AddTrackList");
            if (ImGui::IsItemDeactivated())
            {
                CreateTrackList("TrackList");
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
    ImGui::End();

    ImGui::Begin("Wave");
    ImGui::End();

    ImGui::Begin("Spectrum");
    renderer->Tick(deltaTime);
    
    auto src = audioSourceWindows[0].source;
    if (src->GetSpectrum())
    {
        float values[20];
        for (int i = 0; i < 20; i++)
        {
            values[i] = src->GetSpectrumMagnitude(1000.0 * i);
        }
        ImGui::PlotHistogram("Lines", values, IM_ARRAYSIZE(values), 0, 0, 0, 150, ImGui::GetWindowSize());
    }    
    /*
    WaveMesh* waveMesh = (WaveMesh*)renderer->meshes[0];
    for (int i = 0; i < FT_WINDOW_SIZE / 2; i++)
    {
        float magnitude = audioSourceWindows[0].source->GetSpectrumMagnitude(i);
        waveMesh->SetPeak(i, magnitude / 10);
    }
    */
    ImGui::End();
    
    ShowAudioSourceWindow();
    ShowTrackListWindow();
    ShowListenerWindow();
    
    // Rendering
    ImGui::Render();
    //Sleep(1000);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(glWindow);

    input->Tick(deltaTime);
}
