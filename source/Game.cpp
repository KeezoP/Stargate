//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;


extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false) :
    m_retryAudio(false)

{
m_deviceResources = std::make_unique<DX::DeviceResources>();
m_deviceResources->RegisterDeviceNotify(this);
    }

Game::~Game()
{

    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }

}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{

	m_input.Initialise(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	//setup light
	m_Light.setAmbientColour(0.4f, 0.4f, 0.4f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(5.0f, 7.0f, 0.0f);
	m_Light.setDirection(0.0f, 0.0f, 0.0f);


	//setup camera
	m_Camera01.setPosition(Vector3(10.0f, 0.0f, 0.0f));
	m_Camera01.setRotation(Vector3(-90.0f, -90.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 

    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags |= AudioEngine_Debug;
#endif
    m_audEngine = std::make_unique<AudioEngine>(eflags);
    m_gateOpen = std::make_unique<SoundEffect>(m_audEngine.get(), L"resources/gate-open.wav");
    m_chevron = std::make_unique<SoundEffect>(m_audEngine.get(), L"resources/chevron.wav");
    
    std::random_device rd;
    m_random = std::make_unique<std::mt19937>(rd());
    loopDelay = 2.0f;

   



#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");

    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
    m_chevron-> play
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game
	
	//Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	//Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

	
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{

    // Keezo P: variables to help with Delta-time
    float delta = float(timer.GetElapsedSeconds()); // Delta-time
    int speedScale = 40; // Const to multiply all movements by

    //note that currently.  Delta-time is not considered in the game object movement. 
    // Keezo P: Multiplied movements by delta and speedScale in WASD movements
    if (m_gameInputCommands.left)
    {
        Vector3 rotation = m_Camera01.getRotation();
        rotation.y += (m_Camera01.getRotationSpeed() * delta * speedScale);
        m_Camera01.setRotation(rotation);
    }
    if (m_gameInputCommands.right)
    {
        Vector3 rotation = m_Camera01.getRotation();
        rotation.y -= (m_Camera01.getRotationSpeed() * delta * speedScale);
        m_Camera01.setRotation(rotation);
    }
    if (m_gameInputCommands.forward)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position += (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * delta * speedScale); //add the forward vector
        m_Camera01.setPosition(position);
    }
    if (m_gameInputCommands.back)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position -= (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * delta * speedScale); //add the forward vector
        m_Camera01.setPosition(position);
    }

    if (m_gameInputCommands.strUp)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position.y += 0.2;
        m_Camera01.setPosition(position);
    }

    if (m_gameInputCommands.strDown)
    {
        Vector3 position = m_Camera01.getPosition(); //get the position
        position.y -= 0.2;
        m_Camera01.setPosition(position);
    }

    if (m_gameInputCommands.drag)
    {
        /*
            An attempt was made to rotate the camera using quaternions
            however i did not complete it in time
            
            my initial attempt was to use quaternions to rotate the camera pos
            however i realized that wouldnt work as the axis of rotation would
            be passing through the cameras pos.

            i believe if i had more time, i would use quaternion rotation to
            calculate the cameras 'look at' pos using the cameras pos as the axis location
        */  


        //float sensitivity = 100.0f; 
        ////calc angles for camera rotation
        //float angleX = atan(m_gameInputCommands.dragy/sensitivity) * (180 / XM_PI);
        //float angleY = atan(m_gameInputCommands.dragx/sensitivity) * (180 / XM_PI);

        //Vector3 position = m_Camera01.getPosition();
        //Quaternion cq = rotateCalc(position.x, position.y, position.z, angleX, angleY);
        //m_Camera01.setRotation(DirectX::SimpleMath::Vector3(cq.x, cq.y, cq.z));

    }

    // shows application of quaternion rotation
    if (m_gameInputCommands.quatTest && !cDTrigger) {
        
        // toggles the chevrons into spinning/stationary
        if (chevronSpin) {
            chevronSpin = false;
        }

        else {
            chevronSpin = true;
        }        
    }


    if (chevronSpin) {
        // used to show chevrons spinning using quaternion rotation
        // normally their angle of rotation is fixed, this changes that

        quaternionSpinCounter += 0.225;
    }


        // changes direction of ring, that only work when the dial script isn't running
    if (m_gameInputCommands.direction == true && !cDTrigger) {
        direction *= -1;
        if (direction == 0) {
            direction = -1.0f;
        }

        // and if the ring is stationary, start moving again
        if (SpinRingAngleSpeed != 1) { SpinRingAngleSpeed = 1; }
    }

    // used to toggle the portal on/off
    if (m_gameInputCommands.portal == true && !cDTrigger) {
        
        if (portalTrigger) {
            portalTrigger = false;
        }
        else {
            //m_chevron->Play();
            portalTrigger = true;
        }
    }


    // dialling script, 7 chevrons
    
    if (m_gameInputCommands.beginDial == true && !cDTrigger) {

        // setup for dialling script
        spinCounter = 0; // used for timing how long a wheel should spin
        chevCounter = 0; // used to indicate what chevron is currently involved
        SpinRingAngleSpeed = 0; // stop the ring from spinning
        direction = -1; // set direction of rings spin
        for (int i = 0; i <= 8; i++) { isGlow[i] = 0; } // turn off all chevrons
        cDTrigger = true; // begin script
        pauseRing = true;
        portalTrigger = false;
        chevronSpin = false;
        quaternionSpinCounter = 0;
    }

    

    if (cDTrigger) {

        /*
            Dial Script.
            1) set everything to initial settings, including pause ring spin
            2) spin ring for a bit
            3)stop ring for a bit
            4) fire chevron while ring stopped
            5) changed direction
            5) repeat step 2 until the 7th chevron in the list
            (chevron 1/the top one) has been fired then toggle portal
        */


        int spinSeconds = 0;
        // fire chevrons 2 9 3 8 4 7 1
        // script for which chevron section
        switch (chevCounter) {

            // the start
        case 0:

            // wait time
            spinSeconds = 6;

            // pauses ring for count 0 -> 89 
            if (spinCounter < (30 * spinSeconds) - 1) {
                SpinRingAngleSpeed = 0;
                spinCounter++;
            }

            // starts movement, clears count
            else {

                SpinRingAngleSpeed += 0.02;

                if (SpinRingAngleSpeed >= 1) {
                    // spins ring
                    SpinRingAngleSpeed = 1;
                    chevCounter = 2;
                    spinCounter = 0;
                }
            }
            break;

        // dialControl (spin for how long, next switch case/chevron);
        case 2: dialControl(12, 9); break;
        case 9: dialControl(12, 3); break;
        case 3: dialControl(12, 8); break;
        case 8: dialControl(12, 4); break;
        case 4: dialControl(12, 7); break;
        case 7: dialControl(12, 1); break;
        case 1:
            // spin time
            spinSeconds = 12;
            if (spinCounter > (30 * spinSeconds) - 1 && pauseRing) {

                if (SpinRingAngleSpeed > 0) {
                    SpinRingAngleSpeed -= 0.02;
                }
                else {
                    SpinRingAngleSpeed = 0;
                    isGlow[chevCounter - 1] = 1;
                    pauseRing = false;
                    
                }
            }
            else if (isGlow[0] == 121) {
                m_gateOpen->Play();
            }
            //else if (spinCounter > (30 * (spinSeconds*2)) - 1) {
            else if(isGlow[0] > 301) {
            portalTrigger = true;
                cDTrigger = false;
            }
            spinCounter++;
            break;
        }
    }
    // spins the middle ring using matrix rotation
    SpinRingAngle += (direction * SpinRingAngleSpeed)/4;


    // manual chevron controls, that only work when the dial script isn't running
    if (!cDTrigger) {
        if (m_gameInputCommands.chev1 && isGlow[0] == 0) { isGlow[0] = 1; }
        if (m_gameInputCommands.chev2 && isGlow[1] == 0) { isGlow[1] = 1; }
        if (m_gameInputCommands.chev3 && isGlow[2] == 0) { isGlow[2] = 1; }
        if (m_gameInputCommands.chev4 && isGlow[3] == 0) { isGlow[3] = 1; }
        if (m_gameInputCommands.chev5 && isGlow[4] == 0) { isGlow[4] = 1; }
        if (m_gameInputCommands.chev6 && isGlow[5] == 0) { isGlow[5] = 1; }
        if (m_gameInputCommands.chev7 && isGlow[6] == 0) { isGlow[6] = 1; }
        if (m_gameInputCommands.chev8 && isGlow[7] == 0) { isGlow[7] = 1; }
        if (m_gameInputCommands.chev9 && isGlow[8] == 0) { isGlow[8] = 1; }
        if (m_gameInputCommands.chevClear) { for (int i = 0; i <= 8;  i++) { isGlow[i] = 0; } }

    }
	m_Camera01.Update();	//camera update.

	m_view = m_Camera01.getCameraMatrix();
	m_world = Matrix::Identity;


    //loopDelay -= delta; // delta is time between frames
    //if (loopDelay < 0.0f) {
    //    m_chevronFull->Play();
    //    std::uniform_real_distribution<float>dist(1.0f, 10.0f);
    //    loopDelay = dist(*m_random);
    //}

    




    if (m_retryAudio) {
        m_retryAudio = false;
        if (m_audEngine->Reset()) {
            // TODO restart any sound loops
        }
    }
    else if (!m_audEngine->Update()) {
        if (m_audEngine->IsCriticalError()) {
            m_retryAudio = true;
        }
    }

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                //m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

  
	if (m_input.Quit())
	{
		ExitGame();
	}

    

}

void Game::dialControl(int spinSeconds, int nextChevron) {

    // slows ring to 0 after spinning for spinSeconds
    if (spinCounter > (30 * spinSeconds * 2) - 1 && pauseRing) {

        if (SpinRingAngleSpeed < 0.05 && SpinRingAngleSpeed != 0) {
            SpinRingAngleSpeed = 0;
        }
        if (SpinRingAngleSpeed > 0) {
            SpinRingAngleSpeed -= 0.02;
        }
        else {
            direction *= -1;
            spinCounter = 0;
            
        }
    }

    // fires chevron once ring has been stopped for long enough
    if (SpinRingAngleSpeed == 0 && spinCounter > 15 && pauseRing) {
        isGlow[chevCounter - 1] = 1; 
        pauseRing = false;
        spinCounter = 0;
    }

    // starts spinning ring again after long enough
    else if (spinCounter > 149 && !pauseRing) {
        if (SpinRingAngleSpeed == 0) {
            SpinRingAngleSpeed = 0.04;
        }
        else if (SpinRingAngleSpeed < 1) {
            SpinRingAngleSpeed += 0.02;
        }
        else {
            chevCounter = nextChevron;
            SpinRingAngleSpeed = 1;
            spinCounter = 0;
            pauseRing = true;
        }
    }

    spinCounter++;
}
#pragma endregion


#pragma region Frame Render
// Draws the scene.
void Game::Render()
{	
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Draw Text to the screen
    m_deviceResources->PIXBeginEvent(L"Draw sprite");
    m_sprites->Begin();
		m_font->DrawString(m_sprites.get(), L"2005521 Assignment2 - Stargate", XMFLOAT2(10, 10), Colors::Yellow);
    m_sprites->End();
    m_deviceResources->PIXEndEvent();
	
	//Set Rendering states. 
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());
//	context->RSSetState(m_states->Wireframe());

    int angle; // angle of rotation 
    SimpleMath::Matrix newRotation; // 2nd rotation
    SimpleMath::Matrix newPosition; // translation of object



    // Stargate Model
    m_world = SimpleMath::Matrix::Identity;
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture4.Get(), m_texture4.Get());
    m_BasicModel.Render(context);
    angle = SpinRingAngle;
    newRotation = SimpleMath::Matrix::CreateRotationX(angle * (XM_PI / 180)); // 2nd rotation

    // apply changes
    m_world = SimpleMath::Matrix::Identity;
    m_world *= newRotation; 
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get(), m_texture1.Get());
    m_BasicModel2.Render(context);
    
    /*
        Chevron model render chunk

        Chevron is the name of the 9 objects surrounding the ring.
        Each chevron is made up of 3 models: an 'outer', a 'core' and an 'inner'

        chevrons are created in a loop.
        All chevrons are loaded in at (0,0,0) then translated to (-1.2, 5.9,0)
        
        quaternion rotation used to rotate chevrons 2 -> 9 around the x-axis by -40*
        this rotates the object around the axis but it doesn't rotate their orientation
        so a second (matrix) rotation is applied so that the bottom of the models point inwards
        to the centre of the circle
    */
    

      
    std::vector<ModelClass> modelArray(28); // stores models to use in loop     
    modelArray = {
        m_BasicModel3, m_BasicModel4, m_BasicModel5, m_BasicModel6, m_BasicModel7,
        m_BasicModel8, m_BasicModel9, m_BasicModel10, m_BasicModel11, m_BasicModel12,
        m_BasicModel13,m_BasicModel14, m_BasicModel15, m_BasicModel16, m_BasicModel17,
        m_BasicModel18, m_BasicModel19, m_BasicModel20, m_BasicModel21, m_BasicModel22,
        m_BasicModel23, m_BasicModel24, m_BasicModel25, m_BasicModel26, m_BasicModel27,
        m_BasicModel28, m_BasicModel29 };

    // For loop creating chevrons 

    for (int i = 1; i < 10; i++) {

        angle = (-40 * (i - 1));

            Quaternion XYZ = { 0,0,0,1 };
            
            // if glowing
            if (isGlow[i - 1] > 0) {
                
                // int { 0 1 or 2} {out normal or in}
                
                // calculations
                XYZ = ChevRotateCalc(0.0f, 6.0f, 0.0f, angle, 2,i);
                newPosition = SimpleMath::Matrix::CreateTranslation(XYZ.x, XYZ.y, XYZ.z); // new co-ords

                angle += quaternionSpinCounter;
                newRotation = SimpleMath::Matrix::CreateRotationX(angle * (XM_PI / 180)); // 2nd rotation
                angle -= quaternionSpinCounter;
                // apply changes
                m_world = SimpleMath::Matrix::Identity;
                m_world *= newRotation; 
                m_world *= newPosition;
                m_BasicShaderPair.EnableShader(context);
                m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get(), m_texture1.Get());
                modelArray[i * 3 - 3].Render(context); // outer chevron model

                // inner chevron choice 1 when isglow else choice 1

                // calculations
                XYZ = ChevRotateCalc(0.0f, 6.0f, 0.0f, angle, 0,i);
                newPosition = SimpleMath::Matrix::CreateTranslation(XYZ.x, XYZ.y, XYZ.z); // new co-ords
                angle += quaternionSpinCounter;
                newRotation = SimpleMath::Matrix::CreateRotationX(angle * (XM_PI / 180)); // 2nd rotation
                angle -= quaternionSpinCounter;
                // apply changes
                m_world = SimpleMath::Matrix::Identity;
                m_world *= newRotation; m_world *= newPosition;
                m_BasicShaderPair.EnableShader(context);
                m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture2.Get(), m_texture2.Get());
                modelArray[i * 3 - 1].Render(context); // inner chevron model



                // calculations
                XYZ = ChevRotateCalc(0.0f, 6.0f, 0.0f, angle, 1,i);
                newPosition = SimpleMath::Matrix::CreateTranslation(XYZ.x, XYZ.y, XYZ.z); // new co-ords
                angle += quaternionSpinCounter;
                newRotation = SimpleMath::Matrix::CreateRotationX(angle * (XM_PI / 180)); // 2nd rotation
                angle -= quaternionSpinCounter;
                // apply changes
                m_world = SimpleMath::Matrix::Identity;
                m_world *= newRotation; m_world *= newPosition;
                m_BasicShaderPair.EnableShader(context);
                m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get(), m_texture1.Get());
                modelArray[i * 3 - 2].Render(context); // middle chevron model

                isGlow[i - 1]++;
            }

            // else display normally
            else {

                XYZ = ChevRotateCalc(0.0f, 6.0f, 0.0f, angle, 1,i); // chevron normal
                newPosition = SimpleMath::Matrix::CreateTranslation(XYZ.x, XYZ.y, XYZ.z); // new co-ords
                
                angle += quaternionSpinCounter;
                newRotation = SimpleMath::Matrix::CreateRotationX(angle * (XM_PI / 180)); // 2nd rotation
                angle -= quaternionSpinCounter;
                // apply changes
                m_world = SimpleMath::Matrix::Identity;
                m_world *= newRotation; m_world *= newPosition;

                
                m_BasicShaderPair.EnableShader(context);
                m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get(), m_texture1.Get());
                modelArray[i * 3 - 3].Render(context); // outer chevron model
                m_BasicShaderPair.EnableShader(context);
                m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get(), m_texture1.Get());
                modelArray[i * 3 - 2].Render(context); // middle chevron model
                m_BasicShaderPair.EnableShader(context);
                m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture3.Get(), m_texture3.Get());
                modelArray[i * 3 - 1].Render(context); // inner chevron model





            }
    }
    
    // ramp
    m_world = SimpleMath::Matrix::Identity;
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture5.Get(), m_texture5.Get());
    m_BasicModel30.Render(context);

    // door
    m_world = SimpleMath::Matrix::Identity;
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture6.Get(), m_texture6.Get());
    m_BasicModel31.Render(context);

    if (portalTrigger) {
        // wormhole object in the centre of the rings
        m_world = SimpleMath::Matrix::Identity;
        m_BasicShaderPair.EnableShader(context);
        m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture7.Get(), m_texture7.Get());
        m_BasicModel32.Render(context);
    }

    // walls
    m_world = SimpleMath::Matrix::Identity;
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture4.Get(), m_texture4.Get());
    m_BasicModel33.Render(context);

    // command center
    m_world = SimpleMath::Matrix::Identity;
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture5.Get(), m_texture5.Get());
    m_BasicModel34.Render(context);

    // roof
    m_world = SimpleMath::Matrix::Identity;
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture4.Get(), m_texture4.Get());
    m_BasicModel35.Render(context);

    // floor
    m_world = SimpleMath::Matrix::Identity;
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture5.Get(), m_texture5.Get());
    m_BasicModel36.Render(context);

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

#pragma endregion

DirectX::SimpleMath::Quaternion Game::ChevRotateCalc(float x, float y, float z, int angle, int Choice, int chevID) {

    // int { 0 1 or 2} {out normal or in}

   
    if (isGlow[chevID - 1] == 1) {
        m_chevron->Play();
    }
    if (isGlow[chevID - 1] == 121) {
        m_chevron->Play();
    }



    // for a short period of time, expand chevrons
    if (isGlow[chevID - 1] <= 120) {
        switch (Choice) {

        case 0: y += 0.1f; break;
        case 2: y -= 0.2f; break;

        }
    }

    angle += quaternionSpinCounter;

    // this is for q * p * q-1
    Quaternion point = { x, y, z, 1.0f }; // p
    Quaternion rotChev = SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(1, 0, 0), (angle * -1 * (XM_PI / 180))); // q
    Quaternion invChev = rotChev; invChev.Conjugate();  // q-1
    Quaternion temp = SimpleMath::Quaternion::Concatenate(point, rotChev); // q * p
    temp = SimpleMath::Quaternion::Concatenate(invChev, temp); // temp * q-1

    return (temp);
}
/*
DirectX::SimpleMath::Quaternion Game::rotateCalc(float x, float y, float z, float angle1, float angle2) {
    
    Quaternion cameraPoint = { x, y, z, 1.0f }; // p
    Quaternion quat1 = SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(1, 0, 0), (angle1 * (XM_PI / 180))); // q1
    Quaternion quat2 = SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0, 1, 0), (angle2 * (XM_PI / 180))); // q2
    
    Quaternion quatNormal = SimpleMath::Quaternion::Concatenate(quat1, quat2); // q2 * q1
    Quaternion quatInv = quatNormal; quatInv.Conjugate(); // (q2 * q1) inverted

    Quaternion temp = SimpleMath::Quaternion::Concatenate(cameraPoint, quatNormal); // normal * p
    temp = SimpleMath::Quaternion::Concatenate(quatInv, temp); // temp * inverted
     return (temp);
    
}
*/

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
    m_audEngine->Suspend();
}

void Game::OnResuming()
{
    loopDelay = 2.0f;
    m_timer.ResetElapsedTime();
    m_audEngine->Resume();
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    m_font = std::make_unique<SpriteFont>(device, L"resources/SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

    // stargate is made up of 3 rings, 2 stationary and 1 that spins
    m_BasicModel.InitializeModel(device, "resources/teststargate.obj");
    m_BasicModel2.InitializeModel(device, "resources/SpinRing.obj");

    // stores models to use in loop
    std::vector<ModelClass> chevronModels(28);

    // models 3 -> 29 are chevron components
    chevronModels = {
        m_BasicModel3, m_BasicModel4, m_BasicModel5, m_BasicModel6, m_BasicModel7,
        m_BasicModel8, m_BasicModel9, m_BasicModel10, m_BasicModel11, m_BasicModel12,
        m_BasicModel13,m_BasicModel4, m_BasicModel15, m_BasicModel16, m_BasicModel17,
        m_BasicModel18, m_BasicModel19, m_BasicModel20, m_BasicModel21, m_BasicModel22,
        m_BasicModel23, m_BasicModel24, m_BasicModel25, m_BasicModel26, m_BasicModel27,
        m_BasicModel28, m_BasicModel29 };
   
    m_BasicModel3.InitializeModel(device, "resources/ChevronOuter.obj");
    m_BasicModel4.InitializeModel(device, "resources/ChevronMiddle.obj");
    m_BasicModel5.InitializeModel(device, "resources/ChevronInner.obj");
    
    m_BasicModel6.InitializeModel(device, "resources/ChevronOuter.obj");
    m_BasicModel7.InitializeModel(device, "resources/ChevronMiddle.obj");
    m_BasicModel8.InitializeModel(device, "resources/ChevronInner.obj");
    
    m_BasicModel9.InitializeModel(device, "resources/ChevronOuter.obj");
    m_BasicModel10.InitializeModel(device, "resources/ChevronMiddle.obj");
    m_BasicModel11.InitializeModel(device, "resources/ChevronInner.obj");

    m_BasicModel12.InitializeModel(device, "resources/ChevronOuter.obj");
    m_BasicModel13.InitializeModel(device, "resources/ChevronMiddle.obj");
    m_BasicModel14.InitializeModel(device, "resources/ChevronInner.obj");

    m_BasicModel15.InitializeModel(device, "resources/ChevronOuter.obj");
    m_BasicModel16.InitializeModel(device, "resources/ChevronMiddle.obj");
    m_BasicModel17.InitializeModel(device, "resources/ChevronInner.obj");

    m_BasicModel18.InitializeModel(device, "resources/ChevronOuter.obj");
    m_BasicModel19.InitializeModel(device, "resources/ChevronMiddle.obj");
    m_BasicModel20.InitializeModel(device, "resources/ChevronInner.obj");

    m_BasicModel21.InitializeModel(device, "resources/ChevronOuter.obj");
    m_BasicModel22.InitializeModel(device, "resources/ChevronMiddle.obj");
    m_BasicModel23.InitializeModel(device, "resources/ChevronInner.obj");

    m_BasicModel24.InitializeModel(device, "resources/ChevronOuter.obj");
    m_BasicModel25.InitializeModel(device, "resources/ChevronMiddle.obj");
    m_BasicModel26.InitializeModel(device, "resources/ChevronInner.obj");

    m_BasicModel27.InitializeModel(device, "resources/ChevronOuter.obj");
    m_BasicModel28.InitializeModel(device, "resources/ChevronMiddle.obj");
    m_BasicModel29.InitializeModel(device, "resources/ChevronInner.obj");
    
    m_BasicModel30.InitializeModel(device, "resources/ramp.obj");   
    m_BasicModel31.InitializeModel(device, "resources/Door.obj");
    m_BasicModel32.InitializeModel(device, "resources/portal.obj");
    m_BasicModel33.InitializeModel(device, "resources/roomChunk1.obj");
    m_BasicModel34.InitializeModel(device, "resources/roomChunk2.obj");
    m_BasicModel35.InitializeModel(device, "resources/roomChunk3.obj");
    m_BasicModel36.InitializeModel(device, "resources/roomChunk4.obj");

	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");

// texture setup
   CreateDDSTextureFromFile(device, L"resources/metal.dds", nullptr, m_texture1.ReleaseAndGetAddressOf());
   CreateDDSTextureFromFile(device, L"resources/red.dds", nullptr, m_texture2.ReleaseAndGetAddressOf());
   CreateDDSTextureFromFile(device, L"resources/blue.dds", nullptr, m_texture3.ReleaseAndGetAddressOf());
   CreateDDSTextureFromFile(device, L"resources/grey.dds", nullptr, m_texture4.ReleaseAndGetAddressOf());
   CreateDDSTextureFromFile(device, L"resources/metal_plates.dds", nullptr, m_texture5.ReleaseAndGetAddressOf());
   CreateDDSTextureFromFile(device, L"resources/door.dds", nullptr, m_texture6.ReleaseAndGetAddressOf());
   CreateDDSTextureFromFile(device, L"resources/portal.dds", nullptr, m_texture7.ReleaseAndGetAddressOf());

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f
    );
}


void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
