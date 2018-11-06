var add_button = document.getElementById("add_button")
add_button.addEventListener("click", add_new_user_form)

function add_new_user_form() {
	console.log("add new form")
	var new_form = document.createElement("form")
	new_form.id = "add_user_form"
	new_form.action = "home.html"
	new_form.method = "post"
	
	var table = document.createElement("table")
	table.id = "add_user_table"

	line1 = [text_cell("RFID :", "2"), text_cell("Nom :"), text_cell("Description :"), void_cell()]

	var scan_rfid_button = document.createElement("input")
	scan_rfid_button.type = "submit"
	scan_rfid_button.value = "Scanner le rfid!"
	
	var rfid_tag_form_cell = document.createElement("td")
	var rfid_tag_form = document.createElement("form")
	rfid_tag_form.action = "scan_rfid"
	rfid_tag_form.method = "get"
	rfid_tag_form.appendChild(scan_rfid_button)

	rfid_tag_form_cell.appendChild(rfid_tag_form)

	var submit_new_user_cell = document.createElement("td")
	var submit_new_user = document.createElement("input")
	submit_new_user.type = "submit"
	submit_new_user.value = "Valider"
	submit_new_user_cell.appendChild(submit_new_user)

	line2 = [input_text_cell("rfid"), rfid_tag_form_cell, input_text_cell("name"), input_text_cell("description"), submit_new_user_cell]

	add_table_line(table, line1)
	add_table_line(table, line2)

	new_form.appendChild(table)

	document.getElementById("add_button").disabled = true
	document.getElementById("user_buttons").appendChild(new_form)
}

function add_table_line(table, cells){
	var new_line = document.createElement("tr")
	for (var i=0; i<cells.length;i++){
		new_line.appendChild(cells[i])
	}
	table.appendChild(new_line)
}

function text_cell(text, colspan) {
	var new_cell = document.createElement("td")
	new_cell.innerText = text
	if (colspan) {
		new_cell.colSpan = colspan
	}
	console.log(new_cell)
	return new_cell
}

function void_cell() {
	return document.createElement("td")
}

function input_text_cell(name) {
	var input_text_cell = document.createElement("td")
	var input_text = document.createElement("input")
	input_text.type = "text"
	input_text.name = name
	input_text_cell.appendChild(input_text)
	return input_text_cell
}
