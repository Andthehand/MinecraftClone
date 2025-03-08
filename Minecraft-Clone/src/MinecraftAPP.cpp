#include <RealEngine/Core/EntryPoint.h>
#include <RealEngine.h>

#include "MinecraftLayer.h"

namespace RealEngine {
	class Editor : public Application {
	public:
		Editor(const ApplicationSpecification& specification)
			: Application(specification) {
			PushLayer(new MinecraftLayer());
		}

		~Editor() = default;
	};

	Application* CreateApplication(const ApplicationCommandLineArgs& args) {
		RE_PROFILE_FUNCTION();

		ApplicationSpecification specification = { "Minecraf-Clone", args };
		return new Editor(specification);
	}
}
