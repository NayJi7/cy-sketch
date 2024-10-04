
import tkinter 
from tkinter import filedialog, messagebox
from tkinter import *
import os
import platform
import tkinter.filedialog
import tkinter.messagebox




def open_file(event=None):
    "Ouvrir un fichier à partir de l'onglet Fichier dans l'IDE"
    file_path = filedialog.askopenfilename(filetypes= [("Draw++ Files", "*.dpp")])
    print("%d", file_path) #file_path retourne le chemin exact du fichier selectionné

    #une fois que j'ai récup le chemin, j'ai plus qu'a mettre ca dans la text_area
    if file_path: # on check si on a bien un file_path non vide
        with open(file_path, "r") as file:
            text_area.insert(tkinter.END, file.read())
    else:
        text_area.insert(tkinter.END, "erreur lors de la récupération du fichier")

def run(event=None):
    #résultat de l'appui du bouton run : sauvegarder dans le dossier .to_run
    tkinter.messagebox.showinfo("Run","Exécution du Code Draw++ fait avec succès")
    #on verifie le system d'exploitation sur lequel on est
    if platform.system() == "Linux":
        file_path =  './.to_run/to_execute.dpp'
        os.system("rm ./.to_run/*.dpp")
    elif platform.system() == "Windows":
        tkinter.messagebox.showwarning("Attention","MyDrawpp IDE n'est pas encore compatible avec  " + platform.system())
    else:
        tkinter.messagebox.showerror("Erreur","MyDrawpp IDE ne fonctionne pas avec " + platform.system())
    
    
    print("%d", file_path)
    if file_path: #si le fichier a bien ete enregistrer
        with open(file_path, "w") as file:
            file.write(text_area.get(1.0, tkinter.END)) #je get absolument tout le contenu de ma text_area
        window.title(f"Éditeur de Code Draw++ - {os.path.basename(file_path)}")
   
    print("Running the code...")



def save_as_file(event=None):
#on verifie le system d'exploitation sur lequel on est
    if platform.system() == "Linux":
        if not os.path.isdir("MyDrawings"):
            os.system("mkdir MyDrawings")
        file_path =  tkinter.filedialog.asksaveasfilename(initialdir= "./MyDrawings",filetypes=[("Draw++ Files", "*.dpp")])
    elif platform.system() == "Windows":
        tkinter.messagebox.showwarning("Attention","MyDrawpp IDE n'est pas encore compatible avec  " + platform.system())
    else:
        tkinter.messagebox.showerror("Erreur","MyDrawpp IDE ne fonctionne pas avec " + platform.system())
    
    print("%d", file_path)
    if file_path: #si le fichier a bien ete enregistrer
        with open(file_path, "w") as file:
            file.write(text_area.get(1.0, tkinter.END)) #je get absolument tout le contenu de ma text_area
        window.title(f"Éditeur de Code Draw++ - {os.path.basename(file_path)}")






# je creer le fichier .to_run pour executer les fichiers .dpp avec le compilateur a venir
#je check le système d'exploitation sur lequel l'appli est
if not os.path.isdir(".to_run") and platform.system() == "Linux":
    os.system("mkdir .to_run")
    print("no '.to_run' directory, one has just been created")
elif os.path.isdir(".to_run") and platform.system() == "Windows":
    os.system("mkdir .to_run")
    print("no '.to_run' directory, one has just been created")



#Création de l'envirronement Tkinter
window = tkinter.Tk()
window.title("MyDrawPP IDE")
window.geometry("800x600")

#Gestion du Menu en haut
my_menu = tkinter.Menu(window)
window.config(menu=my_menu)

#Onglet Fichier
file_menu = tkinter.Menu(my_menu, tearoff=False)
my_menu.add_cascade(label="File", menu=file_menu)
file_menu.add_command(label="Open File", command=open_file)
file_menu.add_command(label="Save As", command=save_as_file)


#Bouton run
# run_button = tkinter.Button(my_menu, text="Run", command=run, background="green")
# run_button.pack(side=tkinter.RIGHT)
my_menu.add_command(label="Run",background='green', command=run)

#Zone d'édition de texte
text_area = tkinter.Text(window, wrap="char", undo=True, bg="#353434", fg="white", insertbackground="white", font="Droid", highlightcolor="grey")
text_area.pack(fill=tkinter.BOTH, expand=1)

#shortcuts raccourcis
window.bind('<Control-Shift-S>', save_as_file)
window.bind('<Control-o>', open_file)
window.bind('<Alt-Return>', run)


#lancement de la page 
window.mainloop()
