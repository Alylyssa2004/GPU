import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("resultats_multithread.csv")

for thr in df["Threads"].unique():
    d = df[df["Threads"] == thr]
    #plt.plot(d["N"][:], d["Temps_ns"][:], label=f"Threads={thr}")
    #plt.semilogx(d["N"][:], d["Temps_ns"][:], label=f"Threads={thr}")
    plt.loglog(d["N"][:], d["Temps_ns"][:], label=f"Threads={thr}")

plt.xlabel("N")
plt.ylabel("Temps (ns)")
plt.legend()
plt.tight_layout()
plt.grid()
#plt.savefig("Jolie_courbe_normal.png")  # sauvegarde au lieu de plt.show()
#plt.savefig("Jolie_courbe_log.png")  # sauvegarde au lieu de plt.show()
plt.savefig("Jolie_courbe_log2.png")  # sauvegarde au lieu de plt.show()
plt.close()
