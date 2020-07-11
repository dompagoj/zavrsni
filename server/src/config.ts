export const Config = {
  JWT_KEY: process.env.JWT_KEY!,
  env: {
    value: process.env.NODE_ENV,
    isDev() {
      return this.value === 'dev' || this.value === 'develop' || this.value === 'development'
    },
    isProduction() {
      return this.value === 'prod' || this.value === 'production'
    }
  }
}