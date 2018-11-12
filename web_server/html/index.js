/ ----------------------------------------------------------------------------
// Ajax calls
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// Include HTML files
// ----------------------------------------------------------------------------

function myHTMLInclude() {

    var z, i, a, file, xhttp;
    z = document.getElementsByTagName("*");
    for (i = 0; i < z.length; i++) {
        if (z[i].getAttribute("w3-include-html")) {
            a = z[i].cloneNode(false);
            file = z[i].getAttribute("w3-include-html");
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (xhttp.readyState == 4 && xhttp.status == 200) {
                    a.removeAttribute("w3-include-html");
                    a.innerHTML = xhttp.responseText;
                    z[i].parentNode.replaceChild(a, z[i]);
                    myHTMLInclude();
                }
            }
            xhttp.open("GET", file, true);
            xhttp.send();
            return;
        }
    }
}

// ----------------------------------------------------------------------------
// Main code
// ----------------------------------------------------------------------------

const divAddUserForm = document.getElementById("user_buttons");

const add_button = document.getElementById("add_button");

const userTable = document.getElementById('user_table');

var indexLine = 0;

function addHTMLForm(index = -1) {
    const divForm = document.createElement('div');
    divForm.setAttribute('w3-include-html', 'addForm.html');
    divAddUserForm.appendChild(divForm);
    myHTMLInclude();
    formResponse(index);
}

add_button.onclick = function () {
    addHTMLForm();
};

function formResponse(index) {
    const addUserForm = document.getElementById("add_user_form");
    if (addUserForm) {

        function getFormScanRFID() {
            const formScanRFID = document.getElementById("rfid_tag_form");
            const rfidID = document.getElementById("rfidID");
            if (formScanRFID && rfidID) {

                formScanRFID.addEventListener("submit", function (e) {
                    e.preventDefault();

                    ajaxPost("https://oc-jswebsrv.herokuapp.com/api/lien", "Request ID", function () {
                        ajaxGet("https://oc-jswebsrv.herokuapp.com/api/liens", function (reponse) {
                            var jsonData = JSON.parse(reponse);
                            rfidID.value = jsonData[0].rfid;
                        });
                    }, true);

                });

            } else {
                window.setTimeout(getFormScanRFID, 100);
            }
        }
        getFormScanRFID();


        addUserForm.addEventListener("submit", function (e) {
            e.preventDefault();
            
            var rfid = addUserForm.elements.rfid.value;
            var name = addUserForm.elements.name.value;
            var description = addUserForm.elements.description.value;

            const infosForm = {
                rfid: rfid,
                name: name,
                description: description
            };

            console.log(infosForm);

            addUserForm.remove();

            ajaxPost("https://oc-jswebsrv.herokuapp.com/api/lien", infosForm, function () {
                ajaxGet("https://oc-jswebsrv.herokuapp.com/api/liens", function (reponse) {
                    var jsonData = JSON.parse(reponse);

                    if (index == -1) {

                        //Create DOM elements
                        const line = document.createElement('tr');

                        const name = document.createElement('td');
                        name.id = "name_" + indexLine;
                        name.innerHTML = jsonData[0].name;

                        const rfid = document.createElement('td');
                        rfid.id = "rfid_" + indexLine;
                        rfid.innerHTML = jsonData[0].rfid;

                        const description = document.createElement('td');
                        description.id = "description_" + indexLine;
                        description.innerHTML = jsonData[0].description;

                        const modify = document.createElement('td');
                        const modifyButton = document.createElement('button');

                        modifyButton.onclick = function () {
                            if (addUserForm) {
                                addUserForm.remove();
                            }
                            addHTMLForm(indexLine);
                        };

                        //Add DOM elements                        
                        modify.appendChild(modifyButton);
                        line.appendChild(name);
                        line.appendChild(rfid);
                        line.appendChild(description);
                        line.appendChild(modify);
                        userTable.appendChild(line);

                        indexLine += 1;

                    } else {
                        const name = document.getElementById("name_" + index);
                        name.innerHTML = jsonData[0].name;

                        const rfid = document.getElementById("rfid_" + index);
                        rfid.innerHTML = jsonData[0].rfid;

                        const description = document.getElementById("description_" + index);
                        description.innerHTML = jsonData[0].description;
                    }
                });
            }, true);

        });

    } else {
        window.setTimeout(formResponse, 100);
    }
}
