// Sans la partie Vulkan
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
    cout << "Vector " << a << " (" << n << " first values): ";
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
    // ----- 1. Single thread -----
    auto start = high_resolution_clock::now();   // Début chrono
        vector_add(A, B, C, n);
    auto end = high_resolution_clock::now();    // Fin chrono
    // Temps de calcul
    auto duration_ms = chrono::duration<double, milli>(end - start); cout << "Single thread execution time: " << duration_ms.count() << " ms" << endl;

    // Diviser la tâche pour le multi thread
    int nbThreads = thread::hardware_concurrency();cout << "Cores available " << nbThreads << endl; // nb de coeurs dispo
    vector<thread> threads; int tailleBloc = n / nbThreads; // n calculs avec nbThreads coeurs donne la taille du bloc pour chaque coeur
    
    // ----- 2. Multi thread -----
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
    auto duration_ms2 = chrono::duration<double, milli>(end2 - start2); 
    cout << "Multi thread execution time: " << duration_ms2.count() << " ms" << endl;
    cout << "Single thread execution time: " << duration_ms.count() << " ms" << endl;
    
    // ----- 3. Comparaison Single et Multi thread -----
    double speedup1 = duration_ms.count() / duration_ms2.count();
    cout << "Acceleration factor: " << speedup1 << endl;
    // Résultat
    printvec(C,20,'C');

    // --- 4. Multi-thread avec variation du nombre de threads ---
    vector<int> threads_test = {1, 2, 4, 8, 16}; // nombres de threads à tester

    cout << "\nExecution time as a fonction of the number of threads :" << endl;
    cout << "Threads\tTime (ms)\tAcceleration factor" << endl;

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

        cout << nbThreads << "\t    " << duration_mt.count() << "   \t" << speedup << endl;
    }
    // --- 4b. Faire les courbes ---
    // Rappel - vector<int> threads_test = {1, 2, 4, 8, 16}; // nombres de threads à tester
    size_t n_var[12] = {10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000, 3000000};
    // Faire la comparaison pour chaque N, avec 1,2,4,8,16 threads, enregistrer les temps en ns dans
    // un fichier csv que je pourais traiter plus tard
    // Fichier CSV de sortie
    ofstream csv("Resultats_multithread.csv");
    csv << "N,Threads,Temps_ns\n";

    for (size_t n_test : n_var) {

        // Génération des vecteurs
        vector<int> A(n_test), B(n_test), C(n_test);

        // Remplissage valeurs simples
        for (size_t i = 0; i < n_test; i++) {
            A[i] = i;
            B[i] = 2 * i;
        }

        for (int nbThreads : threads_test) {

            size_t bloc = n_test / nbThreads;

            vector<thread> threads;

            auto start = chrono::high_resolution_clock::now();

            for (int t = 0; t < nbThreads; t++) {
                size_t debut = t * bloc;
                size_t fin = (t == nbThreads - 1) ? n_test : debut + bloc;

                threads.emplace_back(addition, cref(A), cref(B), ref(C), debut, fin);
            }

            for (auto &th : threads) th.join();

            auto end = chrono::high_resolution_clock::now();
            long long duree_ns =
                chrono::duration_cast<chrono::nanoseconds>(end - start).count();

            // Écriture dans le CSV
            csv << n_test << "," << nbThreads << "," << duree_ns << "\n";
        }
    }

    csv.close();
    cout << "\nCSV generated : Resultats_multithread.csv\n";

    return 0;
}