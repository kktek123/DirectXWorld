#pragma once

class Time
{
public:
	static Time* Get();

	static void Create();
	static void Delete();

	static bool Stopped() { return isTimerStopped; }
	static float Delta() { return isTimerStopped ? 0.0f : timeElapsed; }

	void Update();
	void Print();

	void Start();
	void Stop();

	float FPS() const { return framePerSecond; }
	float Running() const { return runningTime; }

private:
	Time(void);
	~Time(void);

	static Time* instance;///< �̱��� ��ü

	static bool isTimerStopped;///< Ÿ�̸� ����
	static float timeElapsed;///< ���� ���������κ��� ����ð�


	INT64 ticksPerSecond;///< �ʴ� ƽī��Ʈ
	INT64 currentTime;///< ���� �ð�
	INT64 lastTime;///< �����ð�
	INT64 lastFPSUpdate;///< ������ FPS ������Ʈ �ð�
	INT64 fpsUpdateInterval;///< fps ������Ʈ ����

	UINT frameCount;///< ������ ��
	float runningTime;///< ���� �ð�
	float framePerSecond;///< FPS
};

///////////////////////////////////////////////////////////////////////////////

class Timer
{
public:
	Timer();
	Timer(const Timer& other);
	~Timer();

	void Start(function<void()> func, int milliSec, UINT repeat = 0);
	void Stop();

	bool Complete() { return bComplete; }
private:
	mutex m;

	bool bComplete = true;
	UINT count = 0;
};

///////////////////////////////////////////////////////////////////////////////

class Performance
{
public:
	Performance();

	void Start();
	float End();

private:
	__int64 tick;
	__int64 start, end;
};

///////////////////////////////////////////////

class AttackTimer
{
	Timer delayTimer;
	Timer attackTimer;

public:
	AttackTimer();
	AttackTimer(const AttackTimer& other);
	//~AttackTimer();

	void Start(UINT delay, UINT attackTime, function<void()> attackStartFunc = NULL, function<void()> attackEndFunc = NULL);
	void Stop();

	bool Attacking() { return attackTimer.Complete() == false; }

};
