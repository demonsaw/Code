
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
	}
	id_left_mousearea
	{
		focus: true;
		onClicked:
		{
			id_setting.open();
			id_chat.inactivate();
		}
	}
	
	// Right
	id_right_icon
	{
		text: fa_refresh;
	}
	id_right_mousearea
	{
		onClicked:
		{
			Client.restart();
		}
	}
}
