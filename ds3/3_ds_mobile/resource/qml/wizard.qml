
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import StatusBar 0.1

Page
{
	id: wizard_root
	visible: true
	title: qsTr("Enigma")

	property alias start_button_background: start_button_background;
	property alias start_button_text: start_button_text;
	property alias back_button: back_button;
	property alias next_button: next_button;

	property int page_index: 0;

	signal colorModified(var color);
	signal pageSwitch(var index, var right);

	onColorModified:
	{
		selected_color = Qt.rgba(color.x, color.y, color.z, color.w);
		id_wizard_statusbar.color = selected_color;
	}

	onPageSwitch:
	{
		// Next Button
		if(right)
		{
			if(index == 0)
			{
				back_button.visible = true
				main_view.push("qrc:/qml/SessionSettings.qml")
			}
			if(index == 1)
			{
				next_button.visible = false
				main_view.push("qrc:/qml/SecuritySettings.qml")
			}

			page_index = index + 1
		}
		// Back Button
		else if(!right)
		{
			if(page_index == 1)
			{
				back_button.visible = false
				main_view.pop()

			}
			if(page_index == 2)
			{
				next_button.visible = true
				main_view.pop()

			}
			page_index = page_index - 1
		}
	}

	// Font
	FontLoader
	{
		source: "qrc:/font/awesome.ttf";
	}

	StatusBar
	{
		id: id_wizard_statusbar
		color: Client.color;
	}

	StackView
	{
		id: main_view
		initialItem: "qrc:/qml/ClientSettings.qml"
		anchors.fill:parent

		// Transitions
		pushEnter: Transition
		{
				PropertyAnimation
				{
					property: "opacity"
					from: 0
					to:1
					duration: 50
				}
		}

		pushExit: Transition
		{
			PropertyAnimation
			{
				property: "opacity"
				from: 1
				to:0
				duration: 50
			}
		}

		popEnter: Transition
		{
			PropertyAnimation
			{
				property: "opacity"
				from: 0
				to:1
				duration: 50
			}
		}

		popExit: Transition
		{
			PropertyAnimation
			{
				property: "opacity"
				from: 1
				to:0
				duration: 50
			}
		}
	}

	footer : Rectangle
	{
		id: footer1;
		anchors.bottom: parent.bottom;

		// Back button (LEFT)
		Button
		{
			id: back_button
			visible:false
			anchors.leftMargin: margin_width
			anchors.rightMargin:margin_width * 6
			anchors.bottomMargin: margin_height
			height: button_height;
			width: parent.width * 0.25
			anchors.left:parent.left
			anchors.bottom:parent.bottom

			contentItem: Text
			{
				color: !back_button.pressed? black : client_text_color
				font.family: "FontAwesome";
				font.pixelSize: font_size
				text: fa_arrow_left + " Back"
				verticalAlignment: Text.AlignVCenter;
				horizontalAlignment: Text.AlignHCenter;
			}

			background: Rectangle
			{
				color: back_button.pressed? selected_color : unselected;
				radius: width / 8;
				border.color: back_button.pressed? mid_grey: lighter_grey;
				border.width: line_size;
			}

			onClicked:
			{
				pageSwitch(page_index, false)
			}
		}

		// Settings button (RIGHT)
		Button
		{
			id: next_button
			anchors.leftMargin: margin_width * 20
			anchors.rightMargin:margin_width
			anchors.bottomMargin: margin_height
			height: button_height;
			width: parent.width * 0.25
			anchors.right:parent.right
			anchors.bottom:parent.bottom

			contentItem: Text
			{
				horizontalAlignment: Text.AlignHCenter
				color: !next_button.pressed? black : client_text_color
				font.family: "FontAwesome"
				font.pixelSize: font_size
				text: "Next " + fa_arrow_right
				verticalAlignment: Text.AlignVCenter
			}

			background: Rectangle
			{
				color: next_button.pressed? selected_color : unselected;
				radius: width / 8;
				border.color: next_button.pressed? mid_grey: lighter_grey;
				border.width: line_size;
			}

			onClicked:
			{
				pageSwitch(page_index, true)
			}
		}

		// Start button
		Button
		{
			id: start_button
			width: parent.width * 0.25
			anchors.leftMargin: margin_width * 2
			anchors.rightMargin: margin_width * 2
			anchors.bottomMargin: margin_height
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom:parent.bottom
			height: button_height;

			contentItem: Text
			{
				id: start_button_text
				horizontalAlignment:Text.AlignHCenter;
				color: client_text_color;
				font.family: "FontAwesome";
				font.pixelSize: font_size ;
				text: "Start"
				verticalAlignment: Text.AlignVCenter;
			}

			background: Rectangle
			{
				id: start_button_background;
				color: selected_color;
				radius: width / 8;
				border.color: start_button.pressed? mid_grey: lighter_grey;
				border.width: line_size;
			}

			onClicked:
			{
				// Set properties
				Client.color = selected_color;
				Client.colorChanged();

				Client.add();
				App.save();

				if (adding_client)
				{
					adding_client = false;
					wizard_active = false;
					id_setting.unloadSetting();
                    Toast.show("Client added");
				}
				else
				{
					adding_client = false;
					wizard_active = false;
					id_stack_view.pop();
				}
			}
		}
	}
}
