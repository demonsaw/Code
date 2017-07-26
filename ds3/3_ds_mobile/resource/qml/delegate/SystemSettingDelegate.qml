
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item
{
	width: parent.width;
	height: header_size;

	property alias system_setting_text: system_setting_text;
	property alias system_setting_icon: system_setting_icon;
	property alias system_checkbox: system_checkbox;

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
			id: system_setting_icon;
			anchors.fill: id_menu;
			color: grey;
			font.family: "FontAwesome";
			font.pixelSize: icon_size;
			height: header_size;
			horizontalAlignment: Text.AlignHCenter;
			text: "";
			verticalAlignment: Text.AlignVCenter;
		}		
	}


	// Setting Text
	Text
	{
		id: system_setting_text;
		anchors
		{
			left: id_menu.right;
			verticalCenter: id_menu.verticalCenter;
		}
		color: grey;
		font.family: "FontAwesome";
		font.pixelSize: title_size;
		height: header_size;
		horizontalAlignment: Text.AlignLeft;
		topPadding: margin_height;
		bottomPadding: margin_height;
		rightPadding:margin_width;
		text: ""
		verticalAlignment: Text.AlignVCenter;
		width: icon_size_2;
	}

	// Checkbox
	Item
	{
		height: header_size;
		width: icon_size_2;
		anchors
		{
			top: parent.top;
			right: parent.right;
			bottom: parent.bottom
		}
		// Setting Checkbox
		CheckBox
		{
			id: system_checkbox;
			anchors
			{
				verticalCenter: parent.verticalCenter;
				horizontalCenter: parent.horizontalCenter;
			}
			font.pixelSize: icon_size;
			height: icon_size;
			scale: scale_ratio;
		}
		Rectangle
		{
			anchors.fill: parent;
			color: "transparent";
		}
	}


	// Row Background
	Rectangle
	{
		id: setting_background;
		anchors.fill: parent;
		z: -1;
		color: light_grey;
		opacity: setting_mouse_area.pressed? 0.25 : 0;
	}

	// Background MouseArea
	MouseArea
	{
		id: setting_mouse_area;
		anchors.fill: setting_background;
		onClicked:
		{
			system_checkbox.checked = !system_checkbox.checked;
		}
	}
}
