#include <iostream>
#include <vector>
#include <chrono>
using namespace std ;;
using namespace std :: chrono ;

// Fonction d'addition et de chronométrage
void vector_add(const vector<int>& A, const vector<int>& B, vector<int>& C, size_t n) {
    auto start = system_clock::now();   // Début chrono
    for (size_t i = 0; i < n; i++) {
        C[i] = A[i] + B[i];
    }
    auto end = system_clock::now();    // Fin chrono
    auto duration = duration_cast<microseconds>(end - start);// Durée en microsecondes
    // Affichage du résultat
    cout << "Temps d'execution : " << duration.count() << " microsecondes" << endl;    // Temps d'exécution
}

int main() {
    // Taille définie dans main (modif ici)
    size_t n = 5;

    // Valeurs modifiables
    vector<int> A = {1, 2, 3, 4, 5};
    vector<int> B = {10, 20, 30, 40, 50};
    vector<int> C(n);
    // Appel de la fonction avec n en paramètre
    vector_add(A, B, C, n);
     cout << "Vecteur C = A + B : ";
    for (size_t i = 0; i < n; i++) {
        cout << C[i] << " ";
    }
    cout << endl;
    return 0;
}