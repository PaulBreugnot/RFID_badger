function ajaxGet(url, callback) {
    var req = new XMLHttpRequest();
    req.open("GET", url);
    req.addEventListener("load", function () {
        if (req.status >= 200 && req.status < 400) {
            // Appelle la fonction callback en lui passant la réponse de la requête
            callback(req.responseText);
        } else {
            console.error(req.status + " " + req.statusText + " " + url);
        }
    });
    req.addEventListener("error", function () {
        console.error("Erreur réseau avec l'URL " + url);
    });
    req.send(null);
}
function ajaxPost(url, data, callback, isJson) {
    var req = new XMLHttpRequest();
    req.open("POST", url);
    req.addEventListener("load", function () {
        if (req.status >= 200 && req.status < 400) {
            callback(req.responseText);
        } else {
            console.error(req.status + " " + req.statusText + " " + url);
        }
    });
    req.addEventListener("error", function () {
        console.error("Erreur réseau avec l'URL " + url);
    });
    if (isJson) {
        req.setRequestHeader("Content-Type", "application/json");
        data = JSON.stringify(data);
    }
    req.send(data);
}

//----------------------------------

var contenuElt = document.createElement("div");

function creerLien(e) {
    var liensElt = document.createElement("div");
    liensElt.classList.add("lien");

    var brElt = document.createElement("br");

    var bElt = document.createElement("b");

    var auteurElt = document.createElement("span");
    auteurElt.textContent = "Ajouté par " + e.auteur;

    var urlElt = document.createElement("span");
    urlElt.textContent = e.url;
    urlElt.style.marginLeft = "8px";

    var titreElt = document.createElement("a");
    titreElt.textContent = e.titre;
    titreElt.href = e.url;
    titreElt.style.color = "#428bca";
    titreElt.style.fontSize = "20px";
    titreElt.style.textDecoration = "none"; 


    
    liensElt.appendChild(bElt);
    liensElt.appendChild(urlElt);
    liensElt.appendChild(auteurElt);
    urlElt.appendChild(brElt);
    bElt.appendChild(titreElt);
    document.getElementById("contenu").appendChild(liensElt);
};

ajaxGet("https://oc-jswebsrv.herokuapp.com/api/liens", function (reponse) {
    var jsonData = JSON.parse(reponse);
    jsonData.forEach(function (lien) {
    creerLien(lien);
});
});

var creationFormulaires = [
{
    placeholder: "Entrez votre nom",
    id: "nom",
    type: "text",
    largeur: "200px"
},
{
    placeholder: "Entrez le titre",
    id: "titre",
    type: "text",
    largeur: "250px"
},
{
    placeholder: "Entrez l'URL du lien",
    id: "url",
    type: "text",
    largeur: "300px"
}];

var bouton = document.getElementById("boutonajouter");
var formulaire = document.createElement("form");
var formulaireDiv = document.getElementById("formulairediv");
formulaireDiv.style.display = "flex";
var boutonAjouter = document.createElement("input");
boutonAjouter.value = "Ajouter";
boutonAjouter.type = "submit";
boutonAjouter.style.height = "24px";

creationFormulaires.forEach(function (e) {
    var spanElt = document.createElement("span");
    var inputElt = document.createElement("input");
    inputElt.placeholder = e.placeholder;
    inputElt.required = true;
    inputElt.id = e.id;
    inputElt.type = e.type;
    inputElt.style.opacity = "1";
    inputElt.style.marginRight = "20px";
    inputElt.style.marginBottom = "10px";
    inputElt.style.width = e.largeur;
    
    spanElt.appendChild(inputElt);
    formulaire.appendChild(spanElt);
});

bouton.addEventListener("click", function() {
    bouton.remove();
    formulaire.appendChild(boutonAjouter);
    formulaireDiv.appendChild(formulaire);
    
});

formulaire.addEventListener("submit", function (e) {
    e.preventDefault();

    var regexHttp = /http:\/\//;
    var regexHttps = /https:\/\//;

    if ((!regexHttp.test(formulaire.elements.url.value)) && ((!regexHttps.test(formulaire.elements.url.value)))) {
        formulaire.elements.url.value = "http://" + formulaire.elements.url.value;
    }
    
    var infosForm = {
    titre: formulaire.elements.titre.value,
    url: formulaire.elements.url.value,
    auteur: formulaire.elements.nom.value
    };

    formulaire.remove();

    ajaxPost("https://oc-jswebsrv.herokuapp.com/api/lien", infosForm, function() {
        ajaxGet("https://oc-jswebsrv.herokuapp.com/api/liens", function (reponse) {
    // Transforme la réponse en tableau d'objets JavaScript
    var jsonData = JSON.parse(reponse);
 
    var newLienElt = document.createElement("div");
    newLienElt.classList.add("lien");
    var brElt = document.createElement("br");
    var bElt = document.createElement("b");

    var auteurElt = document.createElement("span");
    auteurElt.textContent = "Ajouté par " + jsonData[0].auteur;

    var urlElt = document.createElement("span");
    urlElt.textContent = jsonData[0].url;
    urlElt.style.marginLeft = "8px";

    var titreElt = document.createElement("a");
    titreElt.textContent = jsonData[0].titre;
    titreElt.href = jsonData[0].url;
    titreElt.style.color = "#428bca";
    titreElt.style.fontSize = "20px";
    titreElt.style.textDecoration = "none"; 

    var confirmationElt = document.createElement("div");
        confirmationElt.textContent = 'Le lien "' + formulaire.elements.titre.value + '" a bien été ajouté.';
        confirmationElt.style.height = "60px";
        confirmationElt.style.width = "98%";
        confirmationElt.style.backgroundColor = "#ccf2ff";
        confirmationElt.style.color = "#428bca";
        confirmationElt.style.fontWeight = "bold";
        confirmationElt.style.paddingLeft = "20px";
        confirmationElt.style.paddingTop = "30px";
        confirmationElt.style.marginBottom = "10px";
        confirmationElt.style.fontSize = "25px";
        document.querySelector("h1").insertAdjacentElement("afterend", confirmationElt);

        // Il sera effacé après 2 secondes

        setTimeout(function() { 
            confirmationElt.remove();
            }, 2000);

    newLienElt.appendChild(bElt);
    newLienElt.appendChild(urlElt);
    newLienElt.appendChild(auteurElt);
    urlElt.appendChild(brElt);
    bElt.appendChild(titreElt);
    document.getElementById("contenu").insertAdjacentElement('afterbegin', newLienElt);

    var newBouton = document.createElement("button");
    newBouton.id = "boutonajouter";
    newBouton.textContent = "Ajouter un lien";

    document.getElementById("contenu").insertAdjacentElement("afterbegin", newBouton);

    newBouton.addEventListener("click", function () {
    newBouton.remove();
    formulaire.reset();
    formulaireDiv.appendChild(formulaire);
    });   
});
    }, true);
});
    // Merci !