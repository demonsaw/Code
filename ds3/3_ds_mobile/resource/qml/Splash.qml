import QtQuick 2.7
import QtGraphicalEffects 1.0

Item
{
	id: id_splash_root
	
	// Signals
	signal start();
	
	onStart:
	{
		id_fade_cover.start();
	}
	
	Rectangle
	{
		id: id_cover;
		anchors.fill: parent;
		color: "#000000";
		enabled: opacity != 0;
		visible: opacity != 0;
		z: 3;
		NumberAnimation on opacity
		{
			id: id_fade_cover;
			duration: 250;
			from: 1;
			to: 0;
		}
	}

	Rectangle
	{
		anchors.fill: parent;
		color: white;
		enabled: opacity != 0;
		opacity: 5.0;
		visible: opacity != 0;
		z:2;
		Image
		{
			anchors.fill: parent;
			fillMode: Image.PreserveAspectFit;
			source: "qrc:/logo_title.svg";

		}
		NumberAnimation on opacity
		{
			id: id_face_splash;
			running: id_cover.opacity == 0;
			duration: 500;
			from: 3.0;
			to: 0;
		}
	}

}
