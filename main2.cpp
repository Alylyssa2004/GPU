#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <random>   // pour les nombres aléatoires
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <fstream>

using namespace std;;
using namespace std::chrono;

int main() {

    // --- 5. Vulkan ---
    // 1. Test simple : afficher qu’on entre dans le main
    std::cout << "Debut du programme.\n";

    // 2. Déclaration de l'instance Vulkan (non initialisée ou avec VK_NULL_HANDLE)
    VkInstance instance = VK_NULL_HANDLE;

    // 3. Déclaration + initialisation de la structure VkInstanceCreateInfo
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = nullptr;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;

    // 4. Création de l’instance Vulkan
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (result != VK_SUCCESS) {
        std::cerr << "Erreur : Impossible de creer l'instance Vulkan ! Code = "
            << result << std::endl;
        return -1;
    }

    std::cout << "Instance Vulkan creee avec succes !" << std::endl;

    // 5. Destruction de l'instance Vulkan avant la fin du programme
    vkDestroyInstance(instance, nullptr);

    std::cout << "Instance Vulkan detruite. Fin du programme." << std::endl;

    return 0; // Vérifiez que votre programme retourne bien 0

}