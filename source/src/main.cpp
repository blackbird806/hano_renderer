#include "hanoRenderer.hpp"

int main()
{
	{
		hano::Renderer renderer;
		while (true)
		{
			renderer.renderFrame();
		}
	}
    return 0;
}