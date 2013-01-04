#ifndef INPUTINTERFACE_H
#define INPUTINTERFACE_H

class MouseInterface;
class KeyboardInterface;
class GamepadInterface;

class InputInterface
{
public:
	InputInterface() {}
	~InputInterface() {}

private:
	MouseInterface* m_mouse;
	KeyboardInterface* m_keyboard;
	GamepadInterface* m_gamepad;
};

#endif
