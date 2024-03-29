#include "pch.h"
#include "SampleScene.h"

using namespace DirectX;
using namespace winrt;
using namespace winrt::Windows;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Composition;
using namespace winrt::Windows::Graphics::Display;

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
        // UWP on Xbox One triggers a back request whenever the B button is pressed
        // which can result in the app being suspended if unhandled
        auto navigation = SystemNavigationManager::GetForCurrentView();

        navigation.BackRequested([](const winrt::Windows::Foundation::IInspectable&, const BackRequestedEventArgs& args)
            {
                args.Handled(true);
            });
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
        window.SizeChanged({ this, &App::OnWindowSizeChanged });

        auto currentDisplayInformation = DisplayInformation::GetForCurrentView();
        currentDisplayInformation.DpiChanged({ this, &App::OnDpiChanged });

        m_scene->Initialize(window, int(window.Bounds().Width), int(window.Bounds().Height));
    }

protected:
    void OnActivated(CoreApplicationView const& /* applicationView */, IActivatedEventArgs const& /* args */)
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();
        window.Activate();
    }

    void OnWindowSizeChanged(CoreWindow const& sender, WindowSizeChangedEventArgs const& /*args*/)
    {
        m_scene->OnWindowSizeChanged(int(sender.Bounds().Width), int(sender.Bounds().Height));
    }

    void OnDpiChanged(DisplayInformation const& sender, IInspectable const& /*args*/)
    {
        Mouse::SetDpi(sender.LogicalDpi());
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

// Exit helper
void ExitApplication() noexcept
{
    CoreApplication::Exit();
}
