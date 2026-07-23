#pragma once

class Input
{
public:
    static inline bool IsKeyPressed(KeyCode mCode) { return m_pInstance->IsKeyPressed_Impl(mCode); }

protected:
    virtual bool IsKeyPressed_Impl(KeyCode mCode) = 0;

private:
    static Input* m_pInstance;
};

typedef enum KeyCode
{
    KEU_CODE_UNKNOWN = 0,

    KEY_CODE_A = 4,
	KEY_CODE_B = 5,
	KEY_CODE_C = 6,
	KEY_CODE_D = 7,
	KEY_CODE_E = 8,
	KEY_CODE_F = 9,
	KEY_CODE_G = 10,
	KEY_CODE_H = 11,
	KEY_CODE_I = 12,
	KEY_CODE_J = 13,
	KEY_CODE_K = 14,
	KEY_CODE_L = 15,
	KEY_CODE_M = 16,
	KEY_CODE_N = 17,
	KEY_CODE_O = 18,
	KEY_CODE_P = 19,
	KEY_CODE_Q = 20,
	KEY_CODE_R = 21,
	KEY_CODE_S = 22,
	KEY_CODE_T = 23,
	KEY_CODE_U = 24,
	KEY_CODE_V = 25,
	KEY_CODE_W = 26,
	KEY_CODE_X = 27,
	KEY_CODE_Y = 28,
	KEY_CODE_Z = 29,

	KEY_CODE_0 = 30,
	KEY_CODE_1 = 31,
	KEY_CODE_2 = 32,
	KEY_CODE_3 = 33,
	KEY_CODE_4 = 34,
	KEY_CODE_5 = 35,
	KEY_CODE_6 = 36,
	KEY_CODE_7 = 37,
	KEY_CODE_8 = 38,
	KEY_CODE_9 = 39,
} KeyCode;