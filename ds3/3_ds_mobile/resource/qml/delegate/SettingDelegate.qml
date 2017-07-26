
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item
{
	id: setting_item;
	width: parent.width;
	height: header_size;

	property alias setting_text_icon: setting_text_icon
	property alias setting_text: setting_text
	property int setting_index: 0

	// Setting Icon Container
	Rectangle
	{
		id: id_menu;
		anchors.verticalCenter: parent.verticalCenter;
		color: "transparent";
		height: header_size;
		width: icon_size_2;

		// Setting Icon
		Text
		{
			id: setting_text_icon;
			anchors.fill: id_menu;
			color: grey;
			font.family: "FontAwesome";
			font.pixelSize: icon_size;
			horizontalAlignment: Text.AlignHCenter;
			text: "";
			verticalAlignment: Text.AlignVCenter;
		}
	}

	// Setting Text
	Text
	{
		id: setting_text;		
		anchors
		{
			left: id_menu.right;
			verticalCenter: id_menu.verticalCenter;
		}
		color: grey;
		font.family: "FontAwesome";
		font.pixelSize: title_size;
		horizontalAlignment: Text.AlignLeft;
		topPadding: margin_height;
		bottomPadding: margin_height;
		rightPadding:margin_width;
		text: ""
		width: icon_size_2;
	}

	// Row Background
	Rectangle
	{
		id: setting_background;
		anchors.fill: parent;
		z: -1;
		color: light_grey;
		opacity: 0;
	}

	// Background MouseArea
	MouseArea
	{
		id: setting_mouse_area;
		anchors.fill: setting_background;

		onClicked:
		{
			// Bad, cammy. Don't use if statements like that! :/
			// YOLO
			// TODO: Plus these should really be defined as QML enums anyway
			//
			switch (setting_index)
			{
				case 0:
				{
					settings_root.loadSetting("qrc:/qml/ClientSettings.qml");
					break;
				}
				case 1:
				{
					settings_root.loadSetting("qrc:/qml/SecuritySettings.qml");
					break;
				}
				case 2:
				{
					settings_root.loadSetting("qrc:/qml/SessionSettings.qml");
					break;
				}
				case 3:
				{
					settings_root.loadSetting("qrc:/qml/setting/SystemSettings.qml");
					break;
				}
				case 4:
				{
					wizard_active = true;
					adding_client = true;
					App.add();
					settings_root.loadSetting("qrc:/qml/Wizard.qml");
					break;
				}
			}

			settings_root.current_index = setting_index;
		}
		onPressed:
		{
			setting_background.opacity = 0.25;
		}
		onReleased:
		{
			setting_background.opacity = 0;
		}
		onCanceled:
		{
			setting_background.opacity = 0;
		}
	}
}
