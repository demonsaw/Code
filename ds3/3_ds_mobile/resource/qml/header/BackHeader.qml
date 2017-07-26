
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Header
{
	// Left
	id_left_icon
	{
		text: fa_back;
	}
	id_left_mousearea
	{
		onClicked:
		{
			id_setting.unloadSetting();
		}
	}
}
