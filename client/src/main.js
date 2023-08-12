import Vue from 'vue'
import App from './App.vue'
import './registerServiceWorker'
import BluetoothPlugin from "./BluetoothPlugin.js";
import MidiPlugin from "./MidiPlugin.js";

Vue.use(BluetoothPlugin);
Vue.use(MidiPlugin);

Vue.config.productionTip = false

new Vue({
  render: h => h(App),
}).$mount('#app')
