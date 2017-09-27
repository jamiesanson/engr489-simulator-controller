import kivy
kivy.require('1.10.0')
from kivy.app import App
from kivy.uix.boxlayout import BoxLayout

class HomeWidget(BoxLayout):
    pass
    
class SolarApp(App):
    def build(self):
        return HomeWidget()

if __name__ == "__main__":
    SolarApp().run()