#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <random>   // pour les nombres aléatoires
//#include <vulkan/vulkan.h>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <optional>

using namespace std ;;
using namespace std :: chrono ;

// Fonction d'addition et de chronométrage
void vector_add(const vector<int>& A, const vector<int>& B, vector<int>& C, size_t n) {
    for (size_t i = 0; i < n; i++) {
        C[i] = A[i] + B[i];
    }
}

// Version partielle pour le multi thread
void addition(const vector<int>& A,
                       const vector<int>& B,
                       vector<int>& C,
                       size_t debut,
                       size_t fin)
{
    for (size_t i = debut; i < fin; i++) {
        C[i] = A[i] + B[i];
    }
}

void printvec(vector<int>& V, size_t n,char a){
    cout << "Vecteur " << a << " (" << n << " premiers): ";
    for (size_t i = 0; i < n; i++) {
        cout << V[i] << " ";
    }
    cout << endl;
}

int main() {
    size_t n = 3000000;
    vector<int> A(n);
    vector<int> B(n);

    // Création d'un moteur aléatoire
    random_device rd;              // source de "vraie" aléatoire
    mt19937 gen(rd());             // moteur Mersenne Twister
    uniform_int_distribution<> dis(0, 1000); // valeurs entre 0 et 1000

    // Remplissage
    for (size_t i = 0; i < n; i++) {
        A[i] = dis(gen);
        B[i] = dis(gen);
    }

    // Affichage de quelques valeurs pour vérifier
    printvec(A,20,'A');
    printvec(B,20,'B');

    vector<int> C(n);
    // ----- Single thread -----
    auto start = high_resolution_clock::now();   // Début chrono
        vector_add(A, B, C, n);
    auto end = high_resolution_clock::now();    // Fin chrono
    // Temps de calcul
    auto duration_ms = chrono::duration<double, milli>(end - start); cout << "Temps d'execution single thread: " << duration_ms.count() << " ms" << endl;

    // Diviser la tâche pour le multi thread
    int nbThreads = thread::hardware_concurrency();cout << "Nb de coeurs dispo " << nbThreads << endl; // nb de coeurs dispo
    vector<thread> threads; int tailleBloc = n / nbThreads; // n calculs avec nbThreads coeurs donne la taille du bloc pour chaque coeur
    
    // ----- Multi thread -----
    auto start2 = high_resolution_clock::now(); // Début chrono
    for (int t = 0; t < nbThreads; t++){ // Lancer les threads
        int debut = t * tailleBloc;
        int fin = (t == nbThreads - 1) ? n : debut + tailleBloc;
        threads.emplace_back(addition, cref(A), cref(B), ref(C), debut, fin);
    }
    for (auto &th : threads) { // Attendre que les threads aient fini
        th.join();
    }
    auto end2 = high_resolution_clock::now(); // Fin chrono
    // Temps d'execution
    auto duration_ms2 = chrono::duration<double, milli>(end2 - start2); cout << "Temps d'execution multi thread: " << duration_ms2.count() << " ms" << endl;
    
    // Résultat
    printvec(C,20,'C');

    // --- Multi-thread avec variation du nombre de threads ---
    vector<int> threads_test = {1, 2, 4, 8, 16}; // nombres de threads à tester

    cout << "\nTemps d'execution en fonction du nombre de threads :" << endl;
    cout << "Threads \t Temps (ms)\t Facteur d'accélération" << endl;

    for (int nbThreads : threads_test) {
        if (nbThreads > n) nbThreads = (int)n; // ne pas dépasser n

        vector<thread> threads;
        size_t bloc = n / nbThreads;

        auto start_mt = high_resolution_clock::now();
        for (int t = 0; t < nbThreads; t++) {
            size_t debut = t * bloc;
            size_t fin = (t == nbThreads - 1) ? n : debut + bloc;
            threads.emplace_back(addition, cref(A), cref(B), ref(C), debut, fin);
        }
        for (auto &th : threads) th.join();
        auto end_mt = high_resolution_clock::now();
        chrono::duration<double, milli> duration_mt = end_mt - start_mt;

        double speedup = duration_ms.count() / duration_mt.count();

        cout << nbThreads << "\t       \t" << duration_mt.count() << "\t" << speedup << endl;
    }

    return 0;
}