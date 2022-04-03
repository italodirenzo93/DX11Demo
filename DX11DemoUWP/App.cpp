#include "pch.h"
#include "SampleScene.h"

using namespace winrt;
using namespace winrt::Windows;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Composition;

struct App : implements<App, IFrameworkViewSource, IFrameworkView>
{
    App() : m_exit(false), m_visible(true)
    {
    }

    IFrameworkView CreateView()
    {
        return *this;
    }

    void Initialize(CoreApplicationView const& applicationView)
    {
        applicationView.Activated({ this, &App::OnActivated });

        m_scene = std::make_unique<SampleScene>();
    }

    void Load(hstring const&)
    {
        
    }

    void Uninitialize()
    {
        m_scene.reset();
    }

    void Run()
    {
        while (!m_exit)
        {
            if (m_visible)
            {
                m_scene->Tick();

                CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
            }
            else
            {
                CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
            }
        }
    }

    void SetWindow(CoreWindow const& window)
    {
        window.Closed([this](auto&&, auto&&) { m_exit = true; });

        auto windowPtr = static_cast<::IUnknown*>(winrt::get_abi(window));
        m_scene->Initialize(windowPtr, int(window.Bounds().Width), int(window.Bounds().Height));
    }

    void OnActivated(CoreApplicationView const& /* applicationView */, IActivatedEventArgs const& /* args */)
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();
        window.Activate();
    }

private:
    bool m_exit;
    bool m_visible;
    std::unique_ptr<SampleScene> m_scene;
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    CoreApplication::Run(make<App>());
}