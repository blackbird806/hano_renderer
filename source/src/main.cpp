#include "hanoRenderer.hpp"

int main()
{
	{
		hano::Renderer renderer;
		while (renderer.isRunning())
		{
			renderer.renderFrame();
		}
	}
    return 0;
}