#include "nwpwin.h"

class Ship : public vsite::nwp::window 
{
public:
    std::string class_name() override 
    {
        return "static";
    }
};

class main_window : public vsite::nwp::window
{
protected:
    void on_left_button_down(POINT p) override 
    {
        if (!ship) 
        {
            ship.create(*this, WS_CHILD | WS_VISIBLE | SS_CENTER, "X", 0, p.x, p.y, ship_width, ship_height);
        }
        else 
        {
            SetWindowPos(ship, 0, p.x, p.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }

        current_position = p;
    }

    void on_key_up(int vk) override 
    {
        if (!ship)
            return;

        if (is_moving) 
        {
            LONG ex_style = GetWindowLong(ship, GWL_EXSTYLE);
            ex_style = (ex_style & ~WS_EX_CLIENTEDGE) | WS_EX_STATICEDGE;
            SetWindowLong(ship, GWL_EXSTYLE, ex_style);
            SetWindowPos(ship, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            is_moving = false;
            SetWindowText(ship, "X");
        }
    }

    void on_key_down(int vk) override 
    {
        if (!ship)
            return;

        RECT client_rect;
        GetClientRect(*this, &client_rect);

        if (!is_moving) 
        {
            LONG ex_style = GetWindowLong(ship, GWL_EXSTYLE);
            ex_style = (ex_style & ~WS_EX_STATICEDGE) | WS_EX_CLIENTEDGE;
            SetWindowLong(ship, GWL_EXSTYLE, ex_style);
            SetWindowPos(ship, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            is_moving = true;
        }

        int step = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? 10 : 5;

        switch (vk) 
        {
        case VK_LEFT:
            current_position.x = max(current_position.x - step, 0);
            SetWindowText(ship, "<");
            break;
        case VK_RIGHT:
            current_position.x = min(current_position.x + step, client_rect.right - ship_width);
            SetWindowText(ship, ">");
            break;
        case VK_UP:
            current_position.y = max(current_position.y - step, 0);
            SetWindowText(ship, "A");
            break;
        case VK_DOWN:
            current_position.y = min(current_position.y + step, client_rect.bottom - ship_height);
            SetWindowText(ship, "V");
            break;
        default:
            return;
        }

        SetWindowPos(ship, 0, current_position.x, current_position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    void on_destroy() override 
    {
        ::PostQuitMessage(0);
    }

private:
    Ship ship;
    POINT current_position = { 0, 0 };
    static const int ship_width = 20;
    static const int ship_height = 20;
    bool is_moving = false;

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
    vsite::nwp::application app;
    main_window w;
    w.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, "NWP 4");
    return app.run();
}