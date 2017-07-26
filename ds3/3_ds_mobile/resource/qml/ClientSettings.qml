
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import "header"

Page
{
	id: client_settings_root

	// Variables
	property alias name_text: name_text
	property ColorPicker color_picker_form

	// Signals
	signal save();
	
	onSave:
	{
		Client.name = name_text.text;
		Client.nameChanged();
		Client.color = name_text.background.color;
		Client.colorChanged();

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
		id_center_text
		{
			id: header_text;
		}
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

	// Body
	RowLayout
	{
		id: row_layout1
		anchors
		{
			bottomMargin: margin_height_color;
			fill: parent;
			leftMargin: margin_width * 3;
			rightMargin: margin_width * 3;
			topMargin: margin_height * 2;
		}

		Image
		{
			id: enigma_header;
			horizontalAlignment: Text.AlignHCenter;
			enabled: wizard_active && !adding_client;
			visible: wizard_active && !adding_client;
			height: (wizard_active && !adding_client) ? combo_box_height : 0;
			width: (wizard_active && !adding_client) ? combo_box_width : 0;
			anchors
			{
				top: parent.top;
				horizontalCenter: parent.horizontalCenter;
			}

			source: "qrc:/logo_title_home.svg";			
			sourceSize.height: header_size * 1.25;
			fillMode : Image.PreserveAspectFit;
		}

		Component.onCompleted:
		{
			// Default
			name_text.text = Client.name;
		}

		// Name Field
		Item
		{
			id: row1;
			anchors
			{
				left: parent.left;				
				right: parent.right;
				top: wizard_active && !adding_client? enigma_header.bottom : parent.top;
				topMargin: margin_height * 1.5;
			}
			height: combo_box_height;

			TextField
			{
				id: name_text
				anchors.left:parent.left;
				anchors.right:parent.right;
				color: client_text_color;
				font.pixelSize: box_font_size;
				height: combo_box_height;
				inputMethodHints: Qt.ImhNoPredictiveText;
				maximumLength: 24;
				leftPadding: margin_width * 0.75;
				selectionColor: light_blue;
				selectedTextColor: white;

				background: Rectangle
				{
					border.color: light_grey;
					border.width: line_size;
					color: light_grey;
					z: -3;
				}

				onTextChanged:
				{
					if(wizard_active || adding_client)
					{
						Client.name = text;
					}
					header_text.text = text;
				}
			}

		}

		// Color Wheel
		Item
		{
			id: row2
			anchors
			{
				top: row1.bottom;
				left: parent.left;
				right: parent.right;
				bottom: parent.bottom
				topMargin:margin_height * 3;
			}

			ColorPicker
			{
				id:color_picker_form
				anchors.fill: parent
			}
		}
	}
}
