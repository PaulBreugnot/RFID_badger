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
// Include HTML files
// ----------------------------------------------------------------------------

function getCookie(cname) {
    var name = cname + "=";
    var decodedCookie = decodeURIComponent(document.cookie);
    var ca = decodedCookie.split(';');
    for (var i = 0; i < ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
        }
    }
    return "";
}

// ----------------------------------------------------------------------------
// Main code
// ----------------------------------------------------------------------------

const divAddUserForm = document.getElementById("user_buttons");

const add_button = document.getElementById("add_button");

const userTable = document.getElementById('user_table');

var listRFID = [];

var indexLine = 0;

while (getCookie(indexLine + 1)) {
    const infosForm = JSON.parse(getCookie(indexLine + 1));
    addElement(infosForm);
    listRFID.push(infosForm.rfid);
}

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

function addElement(infosForm) {

    //Create DOM elements
    const line = document.createElement('tr');

    const name = document.createElement('td');
    name.id = "name_" + indexLine;
    name.innerHTML = infosForm.name;

    const rfid = document.createElement('td');
    rfid.id = "rfid_" + indexLine;
    rfid.innerHTML = infosForm.rfid;

    const description = document.createElement('td');
    description.id = "description_" + indexLine;
    description.innerHTML = infosForm.description;

    const modify = document.createElement('td');
    const modifyButton = document.createElement('button');

    modifyButton.onclick = function () {
        const addUserForm = document.getElementById("add_user_form");
        if (addUserForm) {
            addUserForm.remove();
        }
        addHTMLForm(infosForm.rfid);
    };

    //Add DOM elements                        
    modify.appendChild(modifyButton);
    line.appendChild(name);
    line.appendChild(rfid);
    line.appendChild(description);
    line.appendChild(modify);
    userTable.appendChild(line);

    indexLine += 1;
}

function formResponse(index) {

    function getAddUserForm() {
        const addUserForm = document.getElementById("add_user_form");

        if (addUserForm) {

            const newIndex = listRFID.indexOf(index);
            const rfid = document.getElementById("rfid_" + listRFID.indexOf(index));
            var newRFID;

            if (index != -1) {
                newRFID = rfid.innerHTML;
                addUserForm.elements.rfid.value = newRFID;
            }

            addUserForm.addEventListener("submit", function (e) {
                e.preventDefault();

                const rfid = addUserForm.elements.rfid.value;
                const name = addUserForm.elements.name.value;
                const description = addUserForm.elements.description.value;

                const infosForm = {
                    rfid: rfid,
                    name: name,
                    description: description
                };

                if (index == -1 && !listRFID.includes(infosForm.rfid)) {

                    addElement(infosForm);

                    listRFID.push(infosForm.rfid);

                    document.cookie = indexLine + "=" + JSON.stringify(infosForm);

                } else if (index == -1 && listRFID.includes(infosForm.rfid)) {
                    alert("Forbidden action: RFID already exists");
                } else {

                    infosForm.rfid = newRFID;

                    const name = document.getElementById("name_" + newIndex);
                    name.innerHTML = infosForm.name;

                    const description = document.getElementById("description_" + newIndex);
                    description.innerHTML = infosForm.description;

                    document.cookie = (listRFID.indexOf(String(newRFID)) + 1) + "=" + JSON.stringify(infosForm);

                }

                addUserForm.remove();

            });

        } else {
            window.setTimeout(getAddUserForm, 100);
        };
    }
    getAddUserForm()
}
