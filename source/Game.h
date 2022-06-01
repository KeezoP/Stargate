//
// Game.h
//
#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Shader.h"
#include "modelclass.h"
#include "Light.h"
#include "Input.h"
#include "Camera.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    
    // Dialing Variables
    int isGlow[9] = { 0,0,0,0,0,0,0,0,0 };
    int spinCounter = 0;
    double glowTimer = 0;
	float SpinRingAngle = 1.0f;
	float SpinRingAngleSpeed = 1.0f;
	float direction = 0.0f;
	bool pauseRing = false;
	bool cDTrigger = false;
	int chevCounter = 0;
	bool portalTrigger = false;
	float quaternionSpinCounter = 0.0f;
	bool chevronSpin = false;

	void OnNewAudioDevice() noexcept { m_retryAudio = true; }

#ifdef DXTK_AUDIO
    void NewAudioDevice();
#endif

    // Properties
    void GetDefaultSize( int& width, int& height ) const;
	
private:

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	}; 

    void Update(DX::StepTimer const& timer);
    void Render();
    void Clear();
	DirectX::SimpleMath::Quaternion ChevRotateCalc(float, float, float, int, int, int);
	//DirectX::SimpleMath::Quaternion rotateCalc(float, float, float, float, float);
	void dialControl(int, int);
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	//input manager. 
	Input									m_input;
	InputCommands							m_gameInputCommands;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;	
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

	// Scene Objects
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

	//lights
	Light																	m_Light;
	Light																	m_Light2;

	//Cameras
	Camera																	m_Camera01;

	//textures 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture3;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture4;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture5;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture6;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture7;


	//Shaders
	Shader																	m_BasicShaderPair;
	ModelClass																m_BasicModel; 
	ModelClass																m_BasicModel2;


	// model 3 - 29 are the chevrons: inner, middle, outer
	ModelClass																m_BasicModel3;
	ModelClass																m_BasicModel4;
	ModelClass																m_BasicModel5;
	ModelClass																m_BasicModel6;
	ModelClass																m_BasicModel7;
	ModelClass																m_BasicModel8;
	ModelClass																m_BasicModel9;
	ModelClass																m_BasicModel10;
	ModelClass																m_BasicModel11;
	ModelClass																m_BasicModel12;
	ModelClass																m_BasicModel13;
	ModelClass																m_BasicModel14;
	ModelClass																m_BasicModel15;
	ModelClass																m_BasicModel16;
	ModelClass																m_BasicModel17;
	ModelClass																m_BasicModel18;
	ModelClass																m_BasicModel19;
	ModelClass																m_BasicModel20;
	ModelClass																m_BasicModel21;
	ModelClass																m_BasicModel22;
	ModelClass																m_BasicModel23;
	ModelClass																m_BasicModel24;
	ModelClass																m_BasicModel25;
	ModelClass																m_BasicModel26;
	ModelClass																m_BasicModel27;
	ModelClass																m_BasicModel28;
	ModelClass																m_BasicModel29;
	ModelClass																m_BasicModel30;
	ModelClass																m_BasicModel31;
	ModelClass																m_BasicModel32;
	ModelClass																m_BasicModel33;
	ModelClass																m_BasicModel34;
	ModelClass																m_BasicModel35;
	ModelClass																m_BasicModel36;



#ifdef DXTK_AUDIO
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;
#endif
	std::unique_ptr<DirectX::AudioEngine> m_audEngine;

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif
	DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;
	bool m_retryAudio;
	std::unique_ptr<DirectX::SoundEffect >m_gateOpen; //= std::unique_ptr<DirectX::SoundEffect>(m_audEngine.get(), L"gate-open.wav");
	std::unique_ptr<DirectX::SoundEffect> m_chevron; //= std::unique_ptr<DirectX::SoundEffect>(m_audEngine.get(), L"chevron.wav");
	std::unique_ptr<std::mt19937> m_random;
	float loopDelay;
};