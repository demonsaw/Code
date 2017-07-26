
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Header
{
	// Left
	id_left_icon
	{
		text: fa_menu;
		font.pixelSize: hamburger_icon_size;
		color: id_left_mousearea.pressed ? light_blue: Client.text_color;
	}
	id_left_mousearea
	{
		onClicked:
		{
			id_setting.close();
		}
	}

	// Center
	id_header
	{
		color: Client.color;
	}
	id_center_text
	{
		text: "Settings"
		color: Client.text_color;
		font.bold: false;
	}

	// Right
	id_right_icon
	{
		text: fa_refresh;
		color: id_right_mousearea.pressed ? light_blue: Client.text_color;
	}
	id_right_mousearea
	{
		onClicked:
		{
			Client.restart();
		}
	}
}
