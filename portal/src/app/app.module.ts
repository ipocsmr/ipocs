import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MatListModule } from '@angular/material/list';
import { MatInputModule } from '@angular/material/input';
import { MatSidenavModule } from '@angular/material/sidenav';
import { MatToolbarModule } from '@angular/material/toolbar';
import { MatTableModule } from '@angular/material/table';
import { MatCardModule } from '@angular/material/card';
import { MatButtonModule } from '@angular/material/button';
import { MatGridListModule } from '@angular/material/grid-list';
import { FlexLayoutModule } from '@angular/flex-layout';
import { MatFormFieldModule, MAT_FORM_FIELD_DEFAULT_OPTIONS } from '@angular/material/form-field';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { FlashArduinoComponent } from './flash-arduino/flash-arduino.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { ConfigurationComponent } from './configuration/configuration.component';
import { EspService } from './esp.service';
import { LoggingComponent } from './logging/logging.component';
import { FlashEspComponent } from './flash-esp/flash-esp.component';

@NgModule({
  declarations: [
    AppComponent,
    ConfigurationComponent,
    FlashArduinoComponent,
    DashboardComponent,
    LoggingComponent,
    FlashEspComponent,
  ],
  imports: [
    MatToolbarModule,
    MatGridListModule,
    MatTableModule,
    MatIconModule,
    MatSidenavModule,
    MatInputModule,
    MatButtonModule,
    MatCardModule,
    MatListModule,
    MatFormFieldModule,
    FlexLayoutModule,
    BrowserModule,
    AppRoutingModule,
    BrowserAnimationsModule
  ],
  providers: [
    EspService,
    {provide: MAT_FORM_FIELD_DEFAULT_OPTIONS, useValue: {floatLabel: 'auto'}}
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
