#pragma once

#include "Event.h"
#include "Code.h"

class KeyEvent : public Event
{
public:
	inline int GetKeyCode() const { return m_keyCode; }

	EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
protected:
	KeyEvent(int keycode)
		: m_keyCode(keycode) {}
	int m_keyCode;
};

class KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(int keyCode, int repeatCount)
		: KeyEvent(keyCode), m_RepeatCount(repeatCount) {}

	inline int GetRepeatCount() const { return m_RepeatCount; }

	virtual std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << m_keyCode << " (" << m_RepeatCount << " repeats";
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::KeyPressed);
private:
	int m_RepeatCount;
};

class KeyReleasedEvent : public KeyEvent
{
public:
	KeyReleasedEvent(int keycode)
		: KeyEvent(keycode) {}

	virtual std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << m_keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::KeyReleased);
};

class KeyTypedEvent : public KeyEvent
{
public:
	KeyTypedEvent(unsigned int keyCode)
		: KeyEvent(keyCode) {
	}

	virtual std::string ToString() const override
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << m_keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::KeyTyped);

};