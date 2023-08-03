#pragma once

class IContainer {
public:
	virtual ~IContainer() {}
	virtual void Initialize() = 0;
	virtual void Destroy() = 0;
	virtual void Setup() = 0;
	virtual void Run() = 0;
	virtual void ProcessInput() = 0;
	virtual void Render() = 0;
};