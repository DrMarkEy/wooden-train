import Vue from 'vue'
import App from './App.vue'
import './registerServiceWorker'
import MidiPlugin from "./MidiPlugin.js";

Vue.use(MidiPlugin);

Vue.config.productionTip = false

new Vue({
  render: h => h(App),
}).$mount('#app')
