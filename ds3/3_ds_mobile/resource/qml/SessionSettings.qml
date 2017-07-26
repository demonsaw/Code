
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import "header"

Page
{
	id: session_settings_root

	// Signals
	signal save();

	onSave:
	{
		Chat.clear();

		Router.address = router_form_text.text;
		Router.port = parseInt(port_text_field.text);
		Router.prime_size = prime_spin_box.value;
		Router.cipher = cipher_text_combo_box.currentText;
		Router.key_size = parseInt(key_size_combo_box.currentText);
		Router.hash = hash_combo_box.currentText;
		Router.iterations = parseInt(iterations_text_field.text);
		Router.salt = salt_text_area.text;

		Client.restart();
		App.save();
        Toast.show("Settings saved");
	}

	// Header
	header: SettingsItemHeader
	{
		id: header
		anchors
		{
			top: parent.top;
			left: parent.left;
			right: parent.right;
		}
		enabled: !wizard_active || adding_client;
		visible: !wizard_active || adding_client;
		height: wizard_active && !adding_client? 0 : header_size;
		width: parent.width;
	}

	// Background
	ColumnLayout 
	{
		id: root

		// Variables
		property alias router_form_text: router_form_text
		property alias port_text_field: port_text_field

		// Margins
		anchors.topMargin: margin_height
		anchors.leftMargin: margin_width
		anchors.rightMargin: margin_width
		anchors.bottomMargin: margin_height

		anchors.fill: parent

		Component.onCompleted:
		{
			// Font
			cipher_text_combo_box.popup.font.pixelSize = box_font_size;
			key_size_combo_box.popup.font.pixelSize = box_font_size;
			hash_combo_box.popup.font.pixelSize = box_font_size;

			// Default
			router_form_text.text = Router.address;
			port_text_field.text = Router.port.toString();
			prime_spin_box.value = Router.prime_size;
			cipher_text_combo_box.currentIndex = cipher_text_combo_box.find(Router.cipher);
			key_size_combo_box.currentIndex = key_size_combo_box.find(Router.key_size.toString());
			hash_combo_box.currentIndex = hash_combo_box.find(Router.hash);
			iterations_text_field.text = Router.iterations.toString();
			salt_text_area.text = Router.salt;
		}

		// Background Mouse Area
		MouseArea
		{
			anchors.fill:parent
			focus:true

			onPressed:
			{
				forceActiveFocus();
			}
		}
		// Router Row
		Item
		{
			id: router_row;
			anchors
			{
				top: parent.top;
				left: parent.left;
				right: parent.right;
				topMargin: margin_height;
			}

            height: screen_height / 8;

			// Router Title
			Text
			{
				id:router_title;
				anchors
				{
					left: parent.left;
					top: parent.top;
				}
				font.pixelSize: box_font_size;
				color: blue;
				width: parent.width * .80;
				text: "Router Address";
			}

			// Port Title
			Text
			{
				id: port_title;
				anchors
				{
					left: parent.left;
					top: parent.top;
					leftMargin: parent.width * 0.80;
				}
				font.pixelSize: box_font_size;
				color: blue;
				width: parent.width * .2;
				text: "Port";
			}

			// Router settings
			Item
			{
				id: routerform_row;
				anchors
				{
					top: router_title.bottom;
					left: parent.left;
					right: parent.right;
				}
				height: parent.height / 2;
				anchors.topMargin: margin_height / 2;

				// Router Address
				TextField
				{
					id: router_form_text;
					anchors.fill: parent;
					height: combo_box_height;
					inputMethodHints: Qt.ImhNoPredictiveText;
					width: parent.width * .80;
					color: grey;
					font.pixelSize: box_font_size;
					leftPadding: margin_width * 0.75;
					selectionColor: light_blue;
					selectedTextColor: white;

					onTextChanged:
					{
						if(wizard_active)
						{
							Router.address = text;
						}
					}
					background: Rectangle
					{
						anchors.fill:parent;
						border.color: border_color;
						border.width: line_size;					
						z: -3;
					}
				}

				// Router port
				TextField
				{
					id: port_text_field;
					anchors
					{
						bottom: parent.bottom;
						top: parent.top;
						right: parent.right;
					}
					height: combo_box_height;
					inputMethodHints: Qt.ImhDigitsOnly;
					leftPadding: margin_width * 0.75;
					width: parent.width * 0.20;
					validator: IntValidator {bottom: 1; top: 65535}
					color: grey;
					font.pixelSize: box_font_size;
					selectionColor: light_blue;
					selectedTextColor: white;

					onTextChanged:
					{
						if(wizard_active)
						{
							Router.port = parseInt(text);
						}
					}
					background: Rectangle
					{
						anchors.fill:parent;
						border.color: border_color;
						border.width: line_size;
						z: -3;
					}
				}
			}
		}

		// Setting Row 0
		Item
		{
		   id: setting_row0;
           height: screen_height / 8;
		   anchors
		   {
			   top: router_row.bottom;
			   left: parent.left;
			   right: parent.right;
			   topMargin: margin_height / 2;
		   }

			// Prime
			Item
			{
				id: center1;
				height:parent.height;
				anchors
				{
					bottom: parent.bottom;
					top: parent.top;
					left: parent.left;
					right: parent.right;
				}

				// Prime Text
				Text
				{
					id: prime_text;
					color: blue;
					text: "Prime Size";
					anchors
					{
						left: parent.left;
						right: parent.right;
					}
					horizontalAlignment: Text.AlignLeft
					font.pixelSize: box_font_size
				}

				// Prime Size Selector (Up/Down)
				SpinBox
				{
					id: prime_spin_box;
					height: combo_box_height;
					anchors
					{
						top: prime_text.bottom;
						left:parent.left;
						right:parent.right;
						topMargin: margin_height / 2;
					}
					down.indicator.width: icon_size * 1.5;
					validator: IntValidator{bottom: 128; top:4096}
					font.pixelSize: box_font_size;
					from: 128;
					to:4096;
					stepSize: 128;
					editable:true;					
					up.indicator.width: icon_size * 1.5;

					background: Rectangle
					{
						anchors.fill:parent;
						border.color: border_color;
						border.width: line_size;
						z: -3;
					}

					onValueChanged:
					{
						if(wizard_active)
							Router.prime_size = value;
					}
				}
			}
		}

		// Setting Row 1
		Item
		{
		   id: setting_row1
           height: screen_height / 8;
		   anchors
		   {
			   top: setting_row0.bottom;
			   left:parent.left;
			   right:parent.right;
			   topMargin:margin_height;
		   }

		   // Left Column: Cipher
		   Item
		   {
			   id: left;
			   anchors
			   {
				   left: parent.left;
				   bottom: parent.bottom;
				   top: parent.top;
			   }

			   height:parent.height;
			   width: parent.width * (2/5);

			   // Cipher Text
			   Text
			   {
				   id: cipher_text;
				   anchors
				   {
					   left: parent.left;
					   top: parent.top;
				   }

				   color: blue;
				   text: qsTr("Cipher");
				   font.pixelSize: box_font_size;
			   }

			   // Cipher ComboBox
			   ComboBox
			   {
                    id: cipher_text_combo_box
                    anchors
                    {
                       top: cipher_text.bottom;
                       right: parent.right;
                       left: parent.left;
                       topMargin: margin_height / 2;
                    }

                    height: combo_box_height

                    // Down Arrow
                    indicator: Canvas
                    {
                        id: id_cipher_canvas;
                        x: parent.width - width - parent.rightPadding
                        y: parent.topPadding + (parent.availableHeight - height) / 2
                        width: margin_width
                        height: margin_width * 0.75
                        contextType: "2d"

                        onPaint:
                        {
                            var context = id_cipher_canvas.getContext("2d");
                            context.reset();
                            context.moveTo(0, 0);
                            context.lineTo(width, 0);
                            context.lineTo(width / 2, height);
                            context.closePath();
                            context.fillStyle = parent.pressed ? blue : grey;
                            context.fill();
                        }
                    }
                    onPressedChanged: id_cipher_canvas.requestPaint();
                    width: combo_box_width
                    font.pixelSize: box_font_size
                    model: ["aes", "mars", "rc6", "serpent", "twofish"]

                    onCurrentTextChanged:
                    {
                        if(wizard_active)
                            Router.cipher = currentText;
                    }
                    onModelChanged:
                    {
                        currentIndex = find(Router.cipher);
                    }
			   }
		   }

		   // Right Column: Key Size
		   Item
		   {
			   id: right;
			   anchors
			   {
				   top: parent.top;
				   right: parent.right;
				   bottom: parent.bottom;
			   }

			   width: parent.width * (2/5);
			   height:parent.height;

			   // Key Size Text
			   Text
			   {
				   id: key_size_text;
				   anchors
				   {
					   left: parent.left;
					   top: parent.top;
				   }

				   color: blue;
				   text: qsTr("Key Size");
				   font.pixelSize: box_font_size;
			   }

			   // Key Size ComboBox
			   ComboBox
			   {
                    id: key_size_combo_box;
                    anchors
                    {
                       top: key_size_text.bottom;
                       right: parent.right;
                       left: parent.left;
                       topMargin: margin_height / 2;
                    }
                    height: combo_box_height;

                    // Down Arrow
                    indicator: Canvas
                    {
                        id: id_key_canvas;
                        x: parent.width - width - parent.rightPadding
                        y: parent.topPadding + (parent.availableHeight - height) / 2
                        width: margin_width
                        height: margin_width * 0.75
                        contextType: "2d"

                        onPaint:
                        {
                            var context = id_key_canvas.getContext("2d");
                            context.reset();
                            context.moveTo(0, 0);
                            context.lineTo(width, 0);
                            context.lineTo(width / 2, height);
                            context.closePath();
                            context.fillStyle = parent.pressed ? blue : grey;
                            context.fill();
                        }
                    }
                    onPressedChanged: id_key_canvas.requestPaint();
                    width: combo_box_width;
                    currentIndex: 2;
                    font.pixelSize: box_font_size;
                    model: ["128", "192", "256"];

                    onCurrentTextChanged:
                    {
                        if(wizard_active)
                            Router.key_size = parseInt(currentText);
                    }

                    onModelChanged:
                    {
                        currentIndex = find(Router.key_size.toString());
                    }
				}
			}
		}

		// Setting Row 2
		Item
		{
		   id: setting_row2;
		   anchors
		   {
			   top: setting_row1.bottom;
			   left: parent.left;
			   right: parent.right;
			   topMargin: margin_height;
		   }

		   width: parent.width;
           height: screen_height / 8;

		   // Left Column: Hash
		   Item
		   {
			   id: left1;
			   anchors
			   {
				   top: parent.top;
				   left:parent.left;
				   bottom: parent.bottom;
			   }

			   height:parent.height;
			   width: parent.width * (2/5);

			   // Hash Text
			   Text
			   {
				   id: hash_text;
				   anchors
				   {
					   left: parent.left;
					   top: parent.top;
				   }

				   color: blue;
				   text: qsTr("Hash");
				   font.pixelSize: box_font_size;
			   }

			   // Hash ComboBox
			   ComboBox
			   {
                    id: hash_combo_box;
                    anchors
                    {
                       top: hash_text.bottom;
                       right: parent.right;
                       left: parent.left;
                       topMargin: margin_height / 2;
                    }
                    height: combo_box_height;

                    // Down Arrow
                    indicator: Canvas
                    {
                        id: id_hash_canvas;
                        x: parent.width - width - parent.rightPadding
                        y: parent.topPadding + (parent.availableHeight - height) / 2
                        width: margin_width
                        height: margin_width * 0.75
                        contextType: "2d"

                        onPaint:
                        {
                            var context = id_hash_canvas.getContext("2d");
                            context.reset();
                            context.moveTo(0, 0);
                            context.lineTo(width, 0);
                            context.lineTo(width / 2, height);
                            context.closePath();
                            context.fillStyle = parent.pressed ? blue : grey;
                            context.fill();
                        }
                    }
                    onPressedChanged: id_hash_canvas.requestPaint();
                    width: combo_box_width;
                    currentIndex: 3;
                    font.pixelSize: box_font_size;
                    model: [ "md5", "sha1", "sha224", "sha256", "sha384", "sha512"];

                    onCurrentTextChanged:
                    {
                        if(wizard_active)
                            Router.hash = currentText;
                    }

                    onModelChanged:
                    {
                        currentIndex = find(Router.hash);
                    }
			   }
		   }

			// Right Column: Iterations
			Item
			{
				id: right1
				anchors
				{
				   bottom: parent.bottom;
				   top: parent.top;
				   right: parent.right;
				}

				height:parent.height;
				width: parent.width * (2/5);

				// Iterations Text
				Text
				{
					id: iterations_text;
					anchors.left: parent.left;
					color: blue;
					text: qsTr("Iterations");
					font.pixelSize: box_font_size;
				}

				// Iterations Text Field
				TextField
				{
					id: iterations_text_field;
					anchors
					{
					   top: iterations_text.bottom;
					   left: parent.left;
					   right: parent.right;
					   topMargin: margin_height / 2;
					}

					validator: IntValidator {bottom: 1; top: 2147483647}
					height: combo_box_height;
					width: combo_box_width;
					leftPadding: margin_width * 0.75;
					font.pixelSize: box_font_size;
					horizontalAlignment: Text.AlignLeft;
					inputMethodHints: Qt.ImhDigitsOnly;
					selectionColor: light_blue;
					selectedTextColor: white;
					text: "";

					background: Rectangle
					{
						border.color: border_color;
						border.width: line_size;
						anchors.fill:parent;
						z: -3;
					}

					onTextChanged:
					{
						if(wizard_active)
							Router.iterations = parseInt(text);
					}
			   }
		   }
		}

		// Setting Row 3 (Salt)
		Item
		{
			id: setting_row3;
			anchors
			{
				top: setting_row2.bottom;
				left: parent.left;
				right: parent.right;
				topMargin: margin_height;
			}

			width: parent.width;
            height: screen_height / 8 * 2;

			// Salt
			Item
			{
				id: center2;
				anchors.fill: parent;
				height:parent.height;

				// Salt Text
				Text
				{
					id: salt_text;
					anchors
					{
						left: parent.left;
						right: parent.right;
					}

					color: blue;
					text: qsTr("Salt");
					horizontalAlignment: Text.AlignLeft;
					font.pixelSize: box_font_size;
				}

				// Salt Text Area
				TextArea
				{
					id: salt_text_area;
					anchors
					{
						top: salt_text.bottom;
						left: parent.left;
						right: parent.right;
						topMargin: margin_height / 2;
					}
					focus: true;
					height: combo_box_height * 2;
					inputMethodHints: Qt.ImhNoPredictiveText;
					placeholderText: qsTr("None");
					font.pixelSize: box_font_size;
					horizontalAlignment: Text.AlignLeft;
					leftPadding: margin_width * 0.75;
					selectionColor: light_blue;
					selectedTextColor: white;
					text: Router.salt;
                    wrapMode: Text.WrapAnywhere;

					background: Rectangle
					{
						border.color: border_color;
						border.width: line_size;
						anchors.fill:parent;
						z: -3;
					}

					onTextChanged:
					{
						if(wizard_active)
							Router.salt = text;
					}
				}
			}
		}
	}
}
