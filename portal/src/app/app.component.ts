import { Component, ViewContainerRef, ViewChildren, QueryList } from '@angular/core';
import { Overlay, OverlayConfig } from '@angular/cdk/overlay';
import { EspService } from './esp.service';
import { Router, NavigationEnd, ActivatedRoute } from '@angular/router';
import { Title } from '@angular/platform-browser';
import { filter, map } from 'rxjs/operators';
import { Version } from '../version'
import { TemplatePortalDirective, Portal, ComponentPortal } from '@angular/cdk/portal';
import { FlashEspComponent } from './flash-esp/flash-esp.component';
import { SpinnerComponent } from './spinner/spinner.component';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  public title: string;
  public version: string = Version;
  private spinnerOverlayRef = this.overlay.create(new OverlayConfig({
    hasBackdrop: true,
    backdropClass: 'dark-backdrop',
    positionStrategy: this.overlay.position().global().centerHorizontally().centerVertically()
  }));

  constructor(private router: Router, private activatedRoute: ActivatedRoute, private titleService: Title,
              private overlay: Overlay, private espService: EspService) {
  }

  ngOnInit() {
    const appTitle = this.titleService.getTitle();
    this.router
      .events.pipe(
        filter(event => event instanceof NavigationEnd),
        map(() => {
          const child = this.activatedRoute.firstChild;
          if (child.snapshot.data['title']) {
            return appTitle + " - " + child.snapshot.data['title'];
          }
          return appTitle;
        })
      ).subscribe((ttl: string) => {
        this.title = ttl;
        this.titleService.setTitle(ttl);
      });
      this.espService.connected$.subscribe((isConnected) => {
        if (isConnected) {
          this.spinnerOverlayRef.detach();
        } else {
          if (!this.spinnerOverlayRef.hasAttached()) {
            const loggingPortal = new ComponentPortal(SpinnerComponent);
            this.spinnerOverlayRef.attach(loggingPortal);
          }
        }
      });
  }
}
