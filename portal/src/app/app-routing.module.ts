import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { ConfigurationComponent } from './configuration/configuration.component';
import { FlashArduinoComponent } from './flash-arduino/flash-arduino.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { LoggingComponent } from './logging/logging.component';
import { FlashEspComponent } from './flash-esp/flash-esp.component';


const routes: Routes = [
  { path: 'dashboard', component: DashboardComponent, data: { title: 'Dashboard' } },
  { path: 'configuration', component: ConfigurationComponent, data: { title: 'Configuration' } },
  { path: 'flash-arduino', component: FlashArduinoComponent, data: { title: 'Flash Arduino' } },
  { path: 'flash-esp', component: FlashEspComponent, data: { title: 'Flash ESP' } },
  { path: 'logging', component: LoggingComponent, data: { title: 'Logging' } },
  { path: '', redirectTo: '/logging', pathMatch: 'full' }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
