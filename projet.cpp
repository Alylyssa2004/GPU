#include <vector>
#include <thread>
#include <iostream>
#include <chrono>
#include <stdexcept>

class Matrice {
protected:
    int lignes;
    int colonnes;
    std::vector<std::vector<double>> data;

public:
    // Constructeur
    Matrice(int nbLignes, int nbColonnes, double valeur) {
        lignes = nbLignes;
        colonnes = nbColonnes;
        data.resize(lignes, std::vector<double>(colonnes, valeur));
    }

    // Accesseur pour le nombre de lignes
    int getNbLignes() const {
        return lignes;
    }

    // Accesseur pour le nombre de colonnes
    int getNbColonnes() const {
        return colonnes;
    }

    // Accesseur pour un élément de la matrice
    double getElement(int ligne, int colonne) const {
        return data[ligne][colonne];
    }

    // Modificateur pour un élément de la matrice
    void setElement(int ligne, int colonne, double valeur) {
        data[ligne][colonne] = valeur;
    }

    // Surcharge de l'opérateur * pour le produit matriciel
    Matrice operator*(const Matrice& B) const {
        // 1. Initialiser la matrice résultat C avec des zéros
        Matrice C(this->lignes, B.colonnes, 0.0);

        // 2. Boucle sur les lignes de A (this)
        for (int i = 0; i < this->lignes; i++) {
            // 3. Boucle sur les colonnes de B
            for (int j = 0; j < B.colonnes; j++) {
                // 4. Initialiser le résultat à 0
                double resultat = 0.0;

                // 5. Boucle sur les colonnes de A (ou lignes de B)
                for (int k = 0; k < this->colonnes; k++) {
                    // 6. Ajouter le produit A(i,k) * B(k,j)
                    resultat += this->data[i][k] * B.data[k][j];
                }

                // 7. Modifier la valeur de C(i,j)
                C.setElement(i, j, resultat);
            }
        }

        // 8. Renvoyer la matrice C
        return C;
    }
};

// Classe de matrice parallèle héritant de Matrice
class MatriceParallele : public Matrice {
public:
    // 2. Utiliser les constructeurs de la classe parente
    using Matrice::Matrice;

    // 3. Spécialisation de l'opérateur * pour calcul parallèle
    MatriceParallele operator*(const Matrice& B) const {
        // 4. Initialiser la matrice résultat C avec des zéros
        MatriceParallele C(this->lignes, B.getNbColonnes(), 0.0);

        // 5. Définir une fonction lambda pour multiplier une ligne
        auto multiplyRow = [&](int row) {
            for (int j = 0; j < B.getNbColonnes(); ++j) {
                double sum = 0.0;
                for (int k = 0; k < this->colonnes; ++k) {
                    sum += this->getElement(row, k) * B.getElement(k, j);
                }
                C.setElement(row, j, sum);
            }
        };

        // 6. Créer un vecteur de threads
        std::vector<std::thread> threads;

        // 7. Boucle sur les lignes de la matrice A
        for (int i = 0; i < this->lignes; ++i) {
            // 8. Ajouter un thread pour chaque ligne
            threads.emplace_back(multiplyRow, i);
        }

        // 9. Rejoindre les threads
        for (auto& thread : threads) {
            thread.join();
        }

        // 10. Retourner la matrice C
        return C;
    }
};

int main() {
    // 1. Définir la taille des matrices (matrices carrées)
    int taille = 500;  

    std::cout << "Test avec des matrices " << taille << "x" << taille << std::endl;
    std::cout << "================================================" << std::endl;

    // 2. Matrices séquentielles
    std::cout << "\nCalcul sequentiel..." << std::endl;
    Matrice A_seq(taille, taille, 1.5);
    Matrice B_seq(taille, taille, 2.0);

    auto debut_seq = std::chrono::high_resolution_clock::now();
    Matrice C_seq = A_seq * B_seq;
    auto fin_seq = std::chrono::high_resolution_clock::now();

    auto duree_seq = std::chrono::duration_cast<std::chrono::milliseconds>(fin_seq - debut_seq);
    std::cout << "Temps d'execution sequentiel : " << duree_seq.count() << " ms" << std::endl;

    // 3. Matrices parallèles
    std::cout << "\nCalcul parallele..." << std::endl;
    MatriceParallele A_par(taille, taille, 1.5);
    MatriceParallele B_par(taille, taille, 2.0);

    auto debut_par = std::chrono::high_resolution_clock::now();
    MatriceParallele C_par = A_par * B_par;
    auto fin_par = std::chrono::high_resolution_clock::now();

    auto duree_par = std::chrono::duration_cast<std::chrono::milliseconds>(fin_par - debut_par);
    std::cout << "Temps d'execution parallele : " << duree_par.count() << " ms" << std::endl;

    // 4. Comparaison des performances
    std::cout << "\n================================================" << std::endl;
    std::cout << "Acceleration : x" << (double)duree_seq.count() / duree_par.count() << std::endl;

    if (duree_par.count() < duree_seq.count()) {
        double gain = ((double)(duree_seq.count() - duree_par.count()) / duree_seq.count()) * 100;
        std::cout << "Le calcul parallele est plus rapide de " << gain << "%" << std::endl;
    } else {
        std::cout << "Le calcul sequentiel est plus rapide pour cette taille" << std::endl;
    }


    return 0;
}