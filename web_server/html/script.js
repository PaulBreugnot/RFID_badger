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

add_button.onclick = function () {
    const divForm = document.createElement('div');
    divForm.setAttribute('w3-include-html', 'form.html');
    divAddUserForm.appendChild(divForm);
    myHTMLInclude();
    formResponse();
};

function formResponse() {
    const addUserForm = document.getElementById("add_user_form");
    if (addUserForm) {
        
        const formScanRFID = document.getElementById("rfid_tag_form");
        
        formScanRFID.style.display = 'flex';

        addUserForm.addEventListener("submit", function (e) {
            e.preventDefault();

            const infosForm = {
                rfid: addUserForm.elements.rfid.value,
                name: addUserForm.elements.name.value,
                description: addUserForm.elements.description.value
            };

            console.log(infosForm);

            addUserForm.remove();
        });

    } else {
        window.setTimeout(formResponse, 100);
    }
}
